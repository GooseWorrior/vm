#include "SyntaxView.h"
#include <ncurses.h>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

using std::regex;
using std::smatch;
using std::string;
using std::vector;

namespace CS246E {
const int PREDIRECT = 0;
const int POSTDIRECT = 1;
const int KEYWORD = 2;
const int PRIMITIVE = 3;
const int SPECIAL = 4;
const int DELPHA = 5;
const int DBETA = 6;
const int STRING = 7;
const int NUMBER = 8;
const int SINGCOMMT = 9;
const int MULTCOMMTELPHA = 10;
const int MULTCOMMTBETA = 11;

const vector<string> keyWord{"if",          "else",         "while",
                             "for",         "do",           "using",
                             "try",         "delete",       "break",
                             "case",        "catch",        "continue",
                             "default",     "dynamic_cast", "using",
                             "static_cast", "friend",       "goto",
                             "new",         "noexcept",     "operator",
                             "private",     "const_cast",   "this",
                             "protected",   "public",       "reinterpret_cast",
                             "return",      "throw",        "typeid",
                             "switch",      "sizeof"};
const vector<string> primitive{
    "auto",    "bool",     "char",     "class",     "const", "decltype",
    "double",  "enum",     "explicit", "extern",    "float", "inline",
    "int",     "long",     "mutable",  "namespace", "short", "signed",
    "static",  "template", "typedef",  "typename",  "union", "unsigned",
    "virtual", "void"};
const vector<string> special{"and",   "or",      "not", "true",
                             "false", "nullptr", "NULL"};
const vector<string> directiveType1{"#define", "#undef", "#if",    "#elif",
                                    "#else",   "#error", "#ifndef"};
const vector<string> directiveType2{"#endif", "#line"};
const vector<char> bracketList{'(', ')', '[', ']', '{', '}'};

SyntaxView::SyntaxView(VM* vm) : View{}, vm{vm} {}

void SyntaxView::initialize() { loadLibrary(); }

void SyntaxView::loadLibrary() {
  for (auto i : special) {
    renderLibrary.push_back(
        pair<int, pair<regex, int>>(SPECIAL, pair<regex, int>(i, 6)));
  }
  for (auto i : keyWord) {
    renderLibrary.push_back(
        pair<int, pair<regex, int>>(KEYWORD, pair<regex, int>(i, 7)));
  }
  for (auto i : primitive) {
    renderLibrary.push_back(
        pair<int, pair<regex, int>>(PRIMITIVE, pair<regex, int>(i, 4)));
  }
  for (auto i : directiveType1) {
    renderLibrary.push_back(pair<int, pair<regex, int>>(
        DELPHA, pair<regex, int>("^\\s*(" + i + ")($|\\s+)", 3)));
  }
  for (auto i : directiveType2) {
    renderLibrary.push_back(pair<int, pair<regex, int>>(
        DBETA, pair<regex, int>("^\\s*(" + i + ")($|\\s+)", 3)));
  }
  renderLibrary.push_back(pair<int, pair<regex, int>>(
      PREDIRECT, pair<regex, int>("^\\s*(#include)\\s*(<|\")", 3)));
  renderLibrary.push_back(pair<int, pair<regex, int>>(
      POSTDIRECT, pair<regex, int>("^\\s*#include\\s*(<.*>|\".*\")", 6)));
  renderLibrary.push_back(
      pair<int, pair<regex, int>>(STRING, pair<regex, int>("\"", 6)));
  renderLibrary.push_back(
      pair<int, pair<regex, int>>(NUMBER, pair<regex, int>("[\\d.]", 6)));
  renderLibrary.push_back(
      pair<int, pair<regex, int>>(SINGCOMMT, pair<regex, int>("\\/\\/", 1)));
  renderLibrary.push_back(pair<int, pair<regex, int>>(
      MULTCOMMTELPHA, pair<regex, int>("\\/\\*", 1)));
  renderLibrary.push_back(pair<int, pair<regex, int>>(
      MULTCOMMTBETA, pair<regex, int>("\\*\\/", 1)));

  // renderLibrary[pair<string, regex>("macro", regex("<.*>"))] = 2;
  // sequence matters!
}

bool SyntaxView::inRange(int row) {
  return row >= vm->WindowPointer.first && row <= vm->WindowPointer.second;
}

void SyntaxView::render(vector<pair<int, int>>& singleComment,
                        vector<pair<bool, pair<int, int>>>& multiComment) {
  vector<int> quoteLoc;
  for (int i = 0; i < vm->text.size(); ++i) {
    smatch m;
    string tempstr;
    int offset = 0;
    bool prev = 0;
    mvchgat(i, 0, -1, A_NORMAL, 0, NULL);
    for (auto it : renderLibrary) {
      tempstr = vm->text[i];
      offset = 0;
      if (!inRange(i) && it.first != MULTCOMMTELPHA &&
          it.first != MULTCOMMTBETA)
        continue;
      while (regex_search(tempstr, m, it.second.first)) {
        if (m.size() > 0) {
          if (it.first == PREDIRECT) {
            pair<int, int> loc1 = transferLoc(i, m.position(1));
            pair<int, int> loc2 = transferLoc(i, m.position(2));
            mvchgat(loc1.first, loc1.second, 8, A_NORMAL, it.second.second,
                    NULL);
            mvchgat(loc2.first, loc2.second, 1, A_NORMAL, 6, NULL);
            break;
          } else if (it.first == POSTDIRECT) {
            pair<int, int> loc = transferLoc(i, m.position(1));
            mvchgat(loc.first, loc.second, m.length(1), A_NORMAL,
                    it.second.second, NULL);
            break;
          } else if (it.first == STRING) {
            quoteLoc.push_back(offset + m.position(0));
          } else if (it.first == DELPHA) {
            pair<int, int> loc = transferLoc(i, m.position(1));
            mvchgat(loc.first, loc.second, -1, A_NORMAL, it.second.second,
                    NULL);
            break;
          } else if (it.first == DBETA) {
            pair<int, int> loc = transferLoc(i, m.position(1));
            mvchgat(loc.first, loc.second, m.length(1), A_NORMAL,
                    it.second.second, NULL);
            break;
          } else if (it.first == SINGCOMMT) {
            singleComment.push_back(pair<int, int>(i, m.position(0) + offset));
          } else if (it.first == MULTCOMMTELPHA) {
            multiComment.push_back(pair<bool, pair<int, int>>(
                false, pair<int, int>(i, m.position(0) + offset)));
          } else if (it.first == MULTCOMMTBETA) {
            multiComment.push_back(pair<bool, pair<int, int>>(
                true, pair<int, int>(i, m.position(0) + offset)));
          } else if (it.first == NUMBER) {
            if (!std::isalpha(vm->text[i][m.position(0) + offset - 1]) &&
                vm->text[i][m.position(0) + offset - 1] != '_' &&
                (m.position() == vm->text[i].size() ||
                 (!std::isalpha(
                      vm->text[i][m.position(0) + offset + m.length(0)]) &&
                  vm->text[i][m.position(0) + offset + m.length(0)] != '_'))) {
              pair<int, int> loc = transferLoc(i, m.position(0) + offset);
              mvchgat(loc.first, loc.second, m.length(0), A_NORMAL,
                      it.second.second, NULL);
            }
          } else {
            if (!std::isalnum(vm->text[i][m.position(0) + offset - 1]) &&
                vm->text[i][m.position(0) + offset - 1] != '_' &&
                (m.position() == vm->text[i].size() ||
                 (!std::isalnum(
                      vm->text[i][m.position(0) + offset + m.length(0)]) &&
                  vm->text[i][m.position(0) + offset + m.length(0)] != '_'))) {
              pair<int, int> loc = transferLoc(i, m.position(0) + offset);
              mvchgat(loc.first, loc.second, m.length(0), A_NORMAL,
                      it.second.second, NULL);
            }
          }
          offset += m.length(0) + m.position(0);
        }
        tempstr = m.suffix().str();
      }
    }
    int frst = -1;
    for (auto it : quoteLoc) {
      if (frst == -1) {
        frst = it;
      } else {
        pair<int, int> loc1 = transferLoc(i, frst);
        pair<int, int> loc2 = transferLoc(i, it);
        mvchgat(loc1.first, loc1.second, loc2.second - loc1.second + 1,
                A_NORMAL, 6, NULL);
        frst = -1;
      }
    }
    if (frst != -1) {
      pair<int, int> loc = transferLoc(i, frst);
      mvchgat(loc.first, loc.second, -1, A_NORMAL, 6, NULL);
    }
    quoteLoc.clear();
    for (int j = 0; j < vm->text[i].size(); ++j) {
      pair<int, int> target{-1, -1};
      if (vm->text[i][j] == ')') {
        target = OpenBracket('(', ')', pair<int, int>(i, j));
        if (target.first == -1) {
          pair<int, int> loc = transferLoc(i, j);
          mvchgat(loc.first, loc.second, 1, A_NORMAL, 2, NULL);
        }
      }
    }
  }
}

pair<int, int> SyntaxView::transferLoc(int r, int c) {
  int row = 0, col = 0, temp1 = 0;
  for (int i = vm->WindowPointer.first; i <= r; ++i) {
    size_t temp2 = 0;
    if (i == r) {
      for (int j = 0; j < c; ++j) {
        temp2 += vm->text[i][j] == '\t' ? 8 : 1;
        temp1 += vm->text[i][j] == '\t' ? 8 : 1;
      }
    } else {
      for (int j = 0; j < vm->text[i].size(); ++j) {
        temp2 += vm->text[i][j] == '\t' ? 8 : 1;
      }
    }
    row += temp2 / vm->WindowSize.second + 1;
  }
  row--;
  col = temp1 % vm->WindowSize.second;
  return pair<int, int>(row, col);
}

void SyntaxView::highlightBracket() {
  pair<int, int> target{-1, -1};
  pair<int, int> local{0, 0};
  if (find(bracketList.begin(), bracketList.end(),
           vm->text[vm->vcursor.getRow()][vm->vcursor.getCol()]) !=
      bracketList.end()) {
    local = pair<int, int>(vm->vcursor.getRow(), vm->vcursor.getCol());
  } else if (vm->vcursor.getCol() > 0 &&
             find(bracketList.begin(), bracketList.end(),
                  vm->text[vm->vcursor.getRow()][vm->vcursor.getCol() - 1]) !=
                 bracketList.end()) {
    local = pair<int, int>(vm->vcursor.getRow(), vm->vcursor.getCol() - 1);
  } else {
    return;
  }
  int bracket = vm->text[local.first][local.second];
  if (bracket == '[') {
    target = CloseBracket('[', ']', local);
  } else if (bracket == '(') {
    target = CloseBracket('(', ')', local);
  } else if (bracket == '{') {
    target = CloseBracket('{', '}', local);
  } else if (bracket == ']') {
    target = OpenBracket('[', ']', local);
  } else if (bracket == ')') {
    target = OpenBracket('(', ')', local);
  } else if (bracket == '}') {
    target = OpenBracket('{', '}', local);
  }
  if (!inRange(target.first)) {
    return;
  } else {
    pair<int, int> loc1 = transferLoc(local.first, local.second);
    pair<int, int> loc2 = transferLoc(target.first, target.second);
    mvchgat(loc1.first, loc1.second, 1, A_NORMAL, 8, NULL);
    mvchgat(loc2.first, loc2.second, 1, A_NORMAL, 8, NULL);
  }
}

pair<int, int> SyntaxView::CloseBracket(char openBracket, char closeBracket,
                                        pair<int, int> local) {
  vector<bool> stack;
  for (size_t i = local.first; i < vm->text.size(); ++i) {
    for (size_t j = local.first == i ? local.second + 1 : 0;
         j < vm->text[i].length(); ++j) {
      if (vm->text[i][j] == closeBracket && stack.size() == 0) {
        return pair<int, int>(i, j);
      } else if (vm->text[i][j] == closeBracket) {
        stack.pop_back();
      } else if (vm->text[i][j] == openBracket) {
        stack.push_back(true);
      }
    }
  }
  return pair<int, int>{-1, -1};
}

pair<int, int> SyntaxView::OpenBracket(char openBracket, char closeBracket,
                                       pair<int, int> local) {
  vector<bool> stack;
  for (int i = local.first; i >= 0; --i) {
    for (int j = local.first == i ? local.second - 1 : vm->text[i].length() - 1;
         j >= 0; --j) {
      if (vm->text[i][j] == openBracket && stack.size() == 0) {
        return pair<int, int>(i, j);
      } else if (vm->text[i][j] == openBracket) {
        stack.pop_back();
      } else if (vm->text[i][j] == closeBracket) {
        stack.push_back(true);
      }
    }
  }
  return pair<int, int>{-1, -1};
}

void SyntaxView::display(pair<int, int> prevPointer, int input,
                         pair<int, int> prevCursor,
                         pair<int, int> prevWindowSize, int prevChar,
                         int prevSize, bool edit) {
  if (prevWindowSize != vm->WindowSize ||
      prevPointer.first < vm->WindowPointer.first ||
      prevPointer.second > vm->WindowPointer.second) {
    printTextAll();
  } else if (vm->text.size() != prevSize) {
    printTextAfterward(input, prevCursor);
  } else if (edit &&
             vm->vcursor.getCol() != vm->text[vm->vcursor.getRow()].size()) {
    printTextLine(input, prevCursor, prevChar);
  } else if (edit) {
    printTextChar(input, prevChar);
  }

  if (vm->vcursor.calculateLine() < vm->WindowSize.first &&
          prevPointer != vm->WindowPointer ||
      prevWindowSize != vm->WindowSize) {
    printPlaceholder();
  }
  update();
}

void SyntaxView::printPlaceholder() {
  attron(COLOR_PAIR(1));
  string placeholderString = "";
  for (int i = vm->vcursor.calculateLine(); i < vm->WindowSize.first - 1; i++) {
    placeholderString += "~\n";
  }
  placeholderString += "~";
  printw("%s", placeholderString.c_str());
  refresh();
  attroff(COLOR_PAIR(1));
}

void SyntaxView::printTextAll() {
  clear();
  /*for (auto i : text) {
    printw("%s\n", i.c_str());
  }*/
  for (size_t i = vm->WindowPointer.first; i <= vm->WindowPointer.second; ++i) {
    printw("%s\n", vm->text[i].c_str());
  }
  refresh();
}

void SyntaxView::printTextAfterward(int input, pair<int, int> prevCursor) {
  clrtobot();
  refresh();
  pair<int, int> loc = vm->updateLoc();
  move(loc.first, 0);
  for (size_t i = vm->vcursor.getRow(); i <= vm->WindowPointer.second; ++i) {
    clrtoeol();
    printw("%s\n", vm->text[i].c_str());
    refresh();
  }
}

void SyntaxView::printTextLine(int input, pair<int, int> prevCursor,
                               int prevChar) {
  if ((input == KEY_BACKSPACE || input == 'x') && prevChar == 0) return;
  pair<int, int> loc = vm->updateLoc();
  if (input == KEY_BACKSPACE || input == 'x') {
    clrtoeol();
    move(loc.first, loc.second);
    for (size_t i = vm->vcursor.getCol();
         i < vm->text[vm->vcursor.getRow()].size(); ++i) {
      addch(vm->text[vm->vcursor.getRow()][i]);
    }
  } else if (vm->state == 1) {
    if (input == '\t') {
      insstr("        ");
    } else {
      insch(input);
    }
  } else if (vm->state == 2) {
    addch(input);
  }
  refresh();
}

void SyntaxView::printTextChar(int input, int prevChar) {
  pair<int, int> loc = vm->updateLoc();
  if (input == KEY_BACKSPACE || input == 'x') {
    move(loc.first, loc.second);
    if (prevChar == '\t')
      addch('\t');
    else if (prevChar != 0)
      addch(' ');
  } else {
    addch(vm->text[vm->vcursor.getRow()][vm->vcursor.getCol() - 1]);
  }
  refresh();
}

void SyntaxView::update() {
  vector<pair<int, int>> singleComment;
  vector<pair<bool, pair<int, int>>> multiComment;
  vector<pair<bool, pair<int, int>>> warningList;
  render(singleComment, multiComment);
  bool flag = false;
  pair<int, int> prev;
  highlightBracket();
  for (auto i : singleComment) {
    if (i.first >= vm->WindowPointer.first &&
        i.first <= vm->WindowPointer.second)
      mvchgat(i.first, i.second, -1, A_NORMAL, 1, NULL);
  }
  std::sort(multiComment.begin(), multiComment.end(),
            [](pair<bool, pair<int, int>> p1, pair<bool, pair<int, int>> p2) {
              return (p1.second.first == p2.second.first)
                         ? p1.second.second < p2.second.second
                         : p1.second.first < p2.second.first;
            });
  for (auto i : multiComment) {
    pair<int, int> loc1 = transferLoc(prev.first, prev.second);
    pair<int, int> loc2 = transferLoc(i.second.first, i.second.second);
    if (flag) {
      if (i.first) {
        if (inRange(prev.first))
          mvchgat(
              loc1.first, loc1.second,
              (loc1.first == loc2.first) ? loc2.second - loc1.second + 2 : -1,
              A_NORMAL, 1, NULL);
        for (int j = std::max<int>(prev.first + 1, vm->WindowPointer.first);
             j <= std::min<int>(i.second.first - 1, vm->WindowPointer.second);
             ++j) {
          pair<int, int> loc3 = transferLoc(j, 0);
          mvchgat(loc3.first, loc3.second, -1, A_NORMAL, 1, NULL);
        }
        if (inRange(i.second.first))
          mvchgat(loc2.first, (loc1.first == loc2.first) ? loc1.second : 0,
                  (loc1.first == loc2.first) ? loc2.second - loc1.second + 2
                                             : loc2.second + 2,
                  A_NORMAL, 1, NULL);
        flag = false;
      } else {
        warningList.push_back(pair<bool, pair<int, int>>(false, loc2));
      }
    } else {
      if (i.first) {
        warningList.push_back(pair<bool, pair<int, int>>(true, loc2));
      } else {
        flag = true;
        prev = i.second;
      }
    }
  }
  if (flag) {
    pair<int, int> loc = transferLoc(prev.first, prev.second);
    if (inRange(prev.first))
      mvchgat(loc.first, loc.second, -1, A_NORMAL, 1, NULL);
    for (int j = std::max<int>(prev.first + 1, vm->WindowPointer.first);
         j <= vm->WindowPointer.second; ++j) {
      pair<int, int> loc3 = transferLoc(j, 0);
      mvchgat(loc3.first, loc3.second, -1, A_NORMAL, 1, NULL);
    }
  }
  for (auto i : warningList) {
    mvchgat(i.second.first, i.second.second, i.first + 1, A_NORMAL, 2, NULL);
  }
}
SyntaxView::~SyntaxView() {}
}  // namespace CS246E
