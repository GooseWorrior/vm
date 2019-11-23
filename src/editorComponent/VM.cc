#include <ncurses.h>
#include <fstream>
#include <iostream>  // remove after debugging
#include <stack>

#include "../Model.h"
#include "../controller/Keyboard.h"
#include "VM.h"

#include "Commandline.h"
#include "Cursor.h"
#include "EditorComponent.h"
#include "StatusLine.h"

using std::stack;

namespace CS246E {

VM::VM(string filename) : vcursor(0, 0, text) {
  // load files
  std::ifstream file{filename};
  file >> std::noskipws;

  char c;
  string line;

  while (file >> c) {
    if (c == '\n') {
      text.push_back(line);
      line.clear();
    } else {
      line += c;
    }
    addch(c);
  }
  text.push_back(line);  // pushes last line
  vcursor.setCursor(text.size() - 1, text.back().length());
  updateWindowSize();
}

void VM::process() {
  int state = 0;  // 0 - readonly, 1 - insert, 2 - command
  pair<int, int> prevCursor;
  int prevSize = 0;
  int input = 0;
  while (input != 'q') {
    input = controller->getChar();
    int prevChar = 0;
    bool edit = false;  // could be omitted
    prevSize = text.size();
    prevCursor = pair<int, int>(vcursor.getRow(), vcursor.getCol());
    switch (input) {
      case 'a':
        state = 1;
        break;
      case KEY_LEFT:
        --vcursor;
        break;
      case KEY_RIGHT:
        ++vcursor;
        break;
      case KEY_UP:
        vcursor.prevLine();
        break;
      case KEY_DOWN:
        vcursor.nextLine();
        break;
      case KEY_BACKSPACE:
        edit = true;
        prevChar = vcursor.erase();
        break;
      case 410:  // special resize character
        break;
      case 36:  // dollar sign $
        if (state == 0) {
          // set to end of line
          vcursor.setCursor(vcursor.getRow(),
                            text[vcursor.getRow()].length() - 1);
        }
        break;
      case 37: {
        char curpos = text[vcursor.getRow()][vcursor.getCol()];
        if (curpos == '[') {
          moveToCloseBracket('[', ']');
        } else if (curpos == '(') {
          moveToCloseBracket('(', ')');
        } else if (curpos == '{') {
          moveToCloseBracket('{', '}');
        } else if (curpos == ']') {
          moveToOpenBracket('[', ']');
        } else if (curpos == ')') {
          moveToOpenBracket('(', ')');
        } else if (curpos == '}') {
          moveToOpenBracket('{', '}');
        } else {
          findPairedBracket();
        }
        break;
      }
      default:
        if (state == 1) {
          edit = true;
          vcursor.insert(input);
        }
        break;
    }
    if (updateWindowSize()) {
      printTextAll();
    } else if (text.size() != prevSize) {
      printTextAfterward(input, prevCursor);
    } else if (edit && vcursor.getCol() != text[vcursor.getRow()].size()) {
      printTextLine(input, prevCursor, prevChar);
    } else if (edit) {
      printTextChar(input, prevChar);
    }

    pair<int, int> loc = updateLoc();
    move(loc.first, loc.second);
  }
}

void VM::moveToCloseBracket(char openBracket, char closeBracket) {
  vector<bool> stack;
  for (size_t i = vcursor.getRow(); i < text.size(); ++i) {
    for (size_t j = vcursor.getRow() == i ? vcursor.getCol() + 1 : 0;
         j < text[i].length(); ++j) {
      if (text[i][j] == closeBracket && stack.size() == 0) {
        vcursor.setCursor(i, j);
        return;
      } else if (text[i][j] == closeBracket) {
        stack.pop_back();
      } else if (text[i][j] == openBracket) {
        stack.push_back(true);
      }
    }
  }
}

void VM::moveToOpenBracket(char openBracket, char closeBracket) {
  vector<bool> stack;
  for (int i = vcursor.getRow(); i >= 0; --i) {
    for (int j = vcursor.getRow() == i ? vcursor.getCol() - 1
                                       : text[i].length() - 1;
         j >= 0; --j) {
      if (text[i][j] == openBracket && stack.size() == 0) {
        vcursor.setCursor(i, j);
        return;
      } else if (text[i][j] == openBracket) {
        stack.pop_back();
      } else if (text[i][j] == closeBracket) {
        stack.push_back(true);
      }
    }
  }
}

void VM::findPairedBracket() {
  vector<pair<int, int>> squareStack;
  vector<pair<int, int>> roundStack;
  vector<pair<int, int>> curlyStack;
  int closest = text[vcursor.getRow()].length() + 1;
  for (size_t i = 0; i < text.size(); ++i) {
    for (size_t j = 0; j < text[i].length(); ++j) {
      if (text[i][j] == '[') {
        squareStack.push_back(std::make_pair(i, j));
      } else if (text[i][j] == '(') {
        roundStack.push_back(std::make_pair(i, j));
      } else if (text[i][j] == '{') {
        curlyStack.push_back(std::make_pair(i, j));
      } else if (text[i][j] == ']') {
        if (squareStack.size()) {
          if (squareStack.back().second > vcursor.getCol() &&
              squareStack.back().first == vcursor.getRow()) {
            closest = closest - vcursor.getCol() >
                              squareStack.back().second - vcursor.getCol()
                          ? squareStack.back().second
                          : closest;
          }
          if (j > vcursor.getCol() && i == vcursor.getRow()) {
            closest =
                closest - vcursor.getCol() > j - vcursor.getCol() ? j : closest;
          }
          squareStack.pop_back();
        }
      } else if (text[i][j] == ')') {
        if (roundStack.size()) {
          if (roundStack.back().second > vcursor.getCol() &&
              roundStack.back().first == vcursor.getRow()) {
            closest = closest - vcursor.getCol() >
                              roundStack.back().second - vcursor.getCol()
                          ? roundStack.back().second
                          : closest;
          }
          if (j > vcursor.getCol() && i == vcursor.getRow()) {
            closest =
                closest - vcursor.getCol() > j - vcursor.getCol() ? j : closest;
          }
          roundStack.pop_back();
        }
      } else if (text[i][j] == '}') {
        if (curlyStack.size()) {
          if (curlyStack.back().second > vcursor.getCol() &&
              curlyStack.back().first == vcursor.getRow()) {
            closest = closest - vcursor.getCol() >
                              curlyStack.back().second - vcursor.getCol()
                          ? curlyStack.back().second
                          : closest;
          }
          if (j > vcursor.getCol() && i == vcursor.getRow()) {
            closest =
                closest - vcursor.getCol() > j - vcursor.getCol() ? j : closest;
          }
          curlyStack.pop_back();
        }
      }
    }
  }
  if (closest != text[vcursor.getRow()].length() + 1) {
    vcursor.setCursor(vcursor.getRow(), closest);
  }
}

bool VM::updateWindowSize() {
  int prevRow = WindowSize.first, prevCol = WindowSize.second;
  getmaxyx(stdscr, WindowSize.first, WindowSize.second);
  return prevRow != WindowSize.first || prevCol != WindowSize.second;
}

pair<int, int> VM::updateLoc() {
  int row = 0, col = 0, temp1 = 0;
  for (size_t i = 0; i <= vcursor.getRow(); ++i) {
    size_t temp2 = 0;
    if (i == vcursor.getRow()) {
      for (size_t j = 0; j < vcursor.getCol(); ++j) {
        temp2 += text[i][j] == '\t' ? 8 : 1;
        temp1 += text[i][j] == '\t' ? 8 : 1;
      }
    } else {
      for (size_t j = 0; j < text[i].size(); ++j) {
        temp2 += text[i][j] == '\t' ? 8 : 1;
      }
    }
    row += temp2 / WindowSize.second + 1;
  }
  row--;
  col = temp1 % WindowSize.second;
  return pair<int, int>(row, col);
}

void VM::printTextAll() {
  clear();
  for (auto i : text) {
    printw("%s\n", i.c_str());
  }
  refresh();
}

void VM::printTextAfterward(int input, pair<int, int> prevCursor) {
  clrtobot();
  refresh();
  for (size_t i = vcursor.getRow(); i < text.size(); ++i) {
    move(i, 0);
    clrtoeol();
    printw("%s\n", text[i].c_str());
    refresh();
  }
}

void VM::printTextLine(int input, pair<int, int> prevCursor, int prevChar) {
  if (input == KEY_BACKSPACE && prevChar == 0) return;
  pair<int, int> loc = updateLoc();
  if (input == KEY_BACKSPACE) {
    clrtoeol();
    move(loc.first, loc.second);
    for (size_t i = vcursor.getCol(); i < text[vcursor.getRow()].size(); ++i) {
      addch(text[vcursor.getRow()][i]);
    }
  } else {
    insch(input);
  }
  refresh();
}

void VM::printTextChar(int input, int prevChar) {
  pair<int, int> loc = updateLoc();
  if (input == KEY_BACKSPACE) {
    move(loc.first, loc.second);
    if (prevChar == '\t')
      addch('\t');
    else if (prevChar != 0)
      addch(' ');
  } else {
    addch(text[vcursor.getRow()][vcursor.getCol() - 1]);
  }
  refresh();
}
}  // namespace CS246E
