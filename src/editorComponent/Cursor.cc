#include "Cursor.h"
#include <math.h>
#include <ncurses.h>
#include <algorithm>
#include <fstream>

using std::min;
using std::pair;
using std::string;
using std::vector;

namespace CS246E {

Cursor::Cursor(int row, int col, vector<string>& theText,
               pair<int, int>& winPtr, pair<int, int>& winSize, int& state)
    : theCursor{row, col},
      theText{theText},
      winPtr{winPtr},
      winSize{winSize},
      stateOffset{-1},
      state{state} {}

int ifNegativeThenZero(int x) { return x >= 0 ? x : 0; }

Cursor& Cursor::operator++() {
  if (theCursor.second <
      ifNegativeThenZero(theText[theCursor.first].size() + stateOffset)) {
    theCursor.second++;
  }
  return *this;
}
Cursor& Cursor::operator--() {
  if (theCursor.second > 0) {
    theCursor.second--;
  }
  return *this;
}

Cursor& Cursor::nextLine() {
  if (theCursor.first == theText.size() - 1) {
    return *this;
  } else if ((theCursor.first - winPtr.first) >=
                 (winPtr.second - winPtr.first) / 3 * 2 &&
             winPtr.second < theText.size() - 1) {
    winPtr.second++;
    updatePointer(-1);

    // int shift = calculateShift();
    // theCursor.first += calculateShift();
  }
  theCursor.second = min<int>(
      theCursor.second,
      ifNegativeThenZero(theText[++theCursor.first].size() + stateOffset));
  return *this;
}
Cursor& Cursor::prevLine() {
  if (theCursor.first == 0) {
    return *this;
  } else if ((theCursor.first - winPtr.first) <=
                 (winPtr.second - winPtr.first) / 3 &&
             winPtr.first > 0) {
    winPtr.first--;
    updatePointer(1);

    // int shift = calculateShift();
    // theCursor.first += calculateShift();
  }
  theCursor.second = min<int>(
      theCursor.second,
      ifNegativeThenZero(theText[--theCursor.first].size() + stateOffset));
  return *this;
}

int Cursor::getRow() { return theCursor.first; }
int Cursor::getCol() { return theCursor.second; }
Cursor& Cursor::insert(int c, int pseudoState) {
  if (c == '\n') {
    theText.insert(theText.begin() + theCursor.first + 1,
                   theText[theCursor.first].substr(
                       theCursor.second, theText[theCursor.first].length()));
    theText[theCursor.first] =
        theText[theCursor.first].substr(0, theCursor.second);
    if (theCursor.first == winPtr.second) {
      winPtr.second++;
      updatePointer(-1);
    } else if (calculateLine() < winSize.first) {
      updatePointer(1);
    }
    theCursor.second = 0;
    theCursor.first++;
    replaceModeDelete.push_back(std::make_pair(theCursor.first, 0));
  } else if (state == 1 || pseudoState == 1) {
    theText[theCursor.first].insert(theCursor.second, 1, c);
    ++(*this);
  } else if (state == 2 || pseudoState == 2) {
    if (theCursor.second == theText[theCursor.first].size()) {
      std::fstream f;
      f.open("debug.txt");
      f << theText[theCursor.first];
      theText[theCursor.first].insert(theCursor.second, 1, c);
      if (!replaceModeDelete.size() ||
          replaceModeDelete.back().first != theCursor.first)
        replaceModeDelete.push_back(
            std::make_pair(theCursor.first, theCursor.second + 1));
    } else {
      theText[theCursor.first][theCursor.second] = c;
      replaceModeDelete.clear();
    }
    ++(*this);
  }
  return *this;
}

bool Cursor::canDelete() {
  for (pair<int, int> canDelete : replaceModeDelete) {
    if (theCursor.first == canDelete.first &&
        theCursor.second >= canDelete.second)
      return true;
  }
  return false;
}

int Cursor::erase(int prevInput, int input, int pseudoState) {
  char prevChar = 0;
  if (theCursor.second == 0 && theCursor.first == 0) {
    return prevChar;
  } else if (theCursor.second == 0) {
    int prevPos =
        ifNegativeThenZero(theText[theCursor.first - 1].size() + stateOffset);
    if (((state == 1 || pseudoState == 1) && input == KEY_BACKSPACE) ||
        (state == 0 && input == 120) || (state == 2 && canDelete())) {
      theText[theCursor.first - 1] += theText[theCursor.first];
      theText.erase(theText.begin() + theCursor.first);
      if (replaceModeDelete.size()) replaceModeDelete.pop_back();
    }
    if ((theCursor.first - winPtr.first) <= (winPtr.second - winPtr.first) &&
        winPtr.first > 0) {
      winPtr.first--;
      updatePointer(1);
    } else if (calculateLine() < winSize.first) {
      updatePointer(1);
    }
    theCursor.first--;
    // theCursor.second = theText[theCursor.first].size();
    theCursor.second = prevPos;
  } else if (((state == 1 || pseudoState == 1) && input == KEY_BACKSPACE) ||
             (state == 0 && input == 120) || (state == 2 && canDelete())) {
    prevChar = theText[theCursor.first][theCursor.second - 1];
    theText[theCursor.first].erase(theCursor.second - 1, 1);
    --(*this);
  } else {
    --(*this);
  }
  return prevChar;
}

// special interations with dot
// const vector<char> insert{'a', 'A', 'i', 'I'};
// const vector<char> special{'s', 'o'};
// const vector<char> custom{'S', 'O'};

int Cursor::handleDot(pair<int, int> lastCommand) {
  if (lastCommand.first == 'r') {  // r needs input
    handler(lastCommand.second);
    return 46;
  }
  if (lastCommand.first == -1) {
    return '\n';  // simulate user input
  }
  if (lastCommand.first == 'R') {
    for (int entry : dot) {
      if (entry == KEY_BACKSPACE) {
        erase('a', KEY_BACKSPACE, 2);
      } else {
        insert(entry, 2);
        // ++operator prevents going past last character, so handles it here
        if (ifNegativeThenZero(theText[theCursor.first].length() - 1) ==
            theCursor.second) {
          ++theCursor.second;
        }
      }
    }
    return 46;
  }
  if (lastCommand.first == 'a' || lastCommand.first == 'A' ||
      lastCommand.first == 'i' || lastCommand.first == 'I' ||
      lastCommand.first == 's' || lastCommand.first == 'S' ||
      lastCommand.first == 'o' || lastCommand.first == 'O') {
    for (int entry : dot) {
      if (entry == KEY_BACKSPACE) {
        erase(entry, KEY_BACKSPACE, 1);
      } else if (entry == -1) {  // insert new line above
        int tmpRow = theCursor.first;
        setCursor(tmpRow, 0);
        insert('\n');
        setCursor(tmpRow, 0);
      } else if (entry == -2) {  // delete row
        theText[theCursor.first] = "";
      } else {
        insert(entry, 1);
        // ++operator prevents going past last character, so handles it here
        if (theText[theCursor.first].length() == 1) {
          ++theCursor.second;
        }
      }
    }
    return 46;
  }

  return lastCommand.first;  // just pipe the command through
}

char Cursor::handleX() {
  if (theCursor.second == 0 || theText[theCursor.first].length() <= 1)
    return 'z';
  char temp = theText[theCursor.first][theCursor.second - 1];
  theText[theCursor.first].erase(theCursor.second - 1, 1);
  --(*this);
  return temp;
}

void Cursor::updatePointer(int mode) {
  if (mode == 1) {
    size_t tempLine = 0;
    for (int i = winPtr.first; i < theText.size(); ++i) {
      size_t tempChar = 0;
      for (int j = 0; j < theText[i].size(); ++j) {
        tempChar += theText[i][j] == '\t' ? 8 : 1;
      }
      tempLine += tempChar / winSize.second + 1;
      if (tempLine >= winSize.first) {
        winPtr.second = i;
        return;
      }
    }
    winPtr.second = theText.size() - 1;
  } else if (mode == -1) {
    size_t tempLine = 0;
    for (int i = winPtr.second; i >= 0; --i) {
      size_t tempChar = 0;
      for (int j = 0; j < theText[i].size(); ++j) {
        tempChar += theText[i][j] == '\t' ? 8 : 1;
      }
      tempLine += tempChar / winSize.second + 1;
      if (tempLine >= winSize.first) {
        winPtr.first = i;
        return;
      }
    }
    winPtr.first = 0;
  } else if (mode == 0) {
    if (winPtr.first > theCursor.first) {
      winPtr.first = theCursor.first;
      updatePointer(1);
    } else if (winPtr.second < theCursor.first) {
      winPtr.second = theCursor.first;
      updatePointer(-1);
    }
  }
}

int Cursor::calculateLine() {
  size_t tempLine = 0;
  for (size_t i = winPtr.first; i <= winPtr.second; ++i) {
    size_t tempChar = 0;
    for (size_t j = 0; j < theText[i].size(); ++j) {
      tempChar += theText[i][j] == '\t' ? 8 : 1;
    }
    tempLine += tempChar / winSize.second + 1;
  }
  return tempLine;
}

void Cursor::setCursor(int x, int y) {
  theCursor.first = x;
  theCursor.second = y;
}

void Cursor::handlePercentage(char input) {
  if (input == '[') {
    moveToCloseBracket('[', ']');
  } else if (input == '(') {
    moveToCloseBracket('(', ')');
  } else if (input == '{') {
    moveToCloseBracket('{', '}');
  } else if (input == ']') {
    moveToOpenBracket('[', ']');
  } else if (input == ')') {
    moveToOpenBracket('(', ')');
  } else if (input == '}') {
    moveToOpenBracket('{', '}');
  } else {
    findPairedBracket();
  }
}

void Cursor::moveToCloseBracket(char openBracket, char closeBracket) {
  vector<bool> stack;
  for (size_t i = theCursor.first; i < theText.size(); ++i) {
    for (size_t j = theCursor.first == i ? theCursor.second + 1 : 0;
         j < theText[i].length(); ++j) {
      if (theText[i][j] == closeBracket && stack.size() == 0) {
        setCursor(i, j);
        return;
      } else if (theText[i][j] == closeBracket) {
        stack.pop_back();
      } else if (theText[i][j] == openBracket) {
        stack.push_back(true);
      }
    }
  }
}

void Cursor::moveToOpenBracket(char openBracket, char closeBracket) {
  vector<bool> stack;
  for (int i = theCursor.first; i >= 0; --i) {
    for (int j = theCursor.first == i ? theCursor.second - 1
                                      : theText[i].length() - 1;
         j >= 0; --j) {
      if (theText[i][j] == openBracket && stack.size() == 0) {
        setCursor(i, j);
        return;
      } else if (theText[i][j] == openBracket) {
        stack.pop_back();
      } else if (theText[i][j] == closeBracket) {
        stack.push_back(true);
      }
    }
  }
}

void Cursor::findPairedBracket() {
  vector<pair<int, int>> squareStack;
  vector<pair<int, int>> roundStack;
  vector<pair<int, int>> curlyStack;
  int closest = theText[theCursor.first].length() + 1;
  int first = 0;
  for (size_t i = 0; i < theText[theCursor.first].length(); ++i) {
  }

  for (size_t i = 0; i < theText.size(); ++i) {
    for (size_t j = 0; j < theText[i].length(); ++j) {
      if (theText[i][j] == '[') {
        squareStack.push_back(std::make_pair(i, j));
      } else if (theText[i][j] == '(') {
        roundStack.push_back(std::make_pair(i, j));
      } else if (theText[i][j] == '{') {
        curlyStack.push_back(std::make_pair(i, j));
      } else if (theText[i][j] == ']') {
        if (theCursor.first == i && theCursor.second < j && !squareStack.size())
          return;
        checkClosest(squareStack, closest, i, j);
      } else if (theText[i][j] == ')') {
        if (theCursor.first == i && theCursor.second < j && !roundStack.size())
          return;
        checkClosest(roundStack, closest, i, j);
      } else if (theText[i][j] == '}') {
        if (theCursor.first == i && theCursor.second < j && !curlyStack.size())
          return;
        checkClosest(curlyStack, closest, i, j);
      }
    }
  }
  if (closest != theText[theCursor.first].length() + 1) {
    for (size_t i = theCursor.second; i < closest; ++i) {
      if (theText[theCursor.first][i] == '{' ||
          theText[theCursor.first][i] == '[' ||
          theText[theCursor.first][i] == '(')
        return;
    }
    setCursor(theCursor.first, closest);
  }
}

int Cursor::checkClosest(vector<pair<int, int>>& stack, int& closest, int i,
                         int j) {
  if (stack.size()) {
    if (stack.back().second > theCursor.second &&
        stack.back().first == theCursor.first &&
        closest - theCursor.second > stack.back().second - theCursor.second) {
      closest = stack.back().second;
    }
    if (j > theCursor.second && i == theCursor.first &&
        closest - theCursor.second > j - theCursor.second) {
      closest = j;
    }
    stack.pop_back();
  }
}

void Cursor::handleCaret() {
  size_t index;
  for (index = 0; index < theText[theCursor.first].length(); ++index) {
    if (theText[theCursor.first][index] != ' ' &&
        theText[theCursor.first][index] != '\t') {
      break;
    }
  }
  if (index == theText[theCursor.first].length()) {
    setCursor(theCursor.first, index - 1);
  } else {
    setCursor(theCursor.first, index);
  }
}

void Cursor::handlef(int toFind) {
  lastFind.first = 'f';
  lastFind.second = toFind;
  size_t index;
  for (index = theCursor.second + 1; index < theText[theCursor.first].length();
       ++index) {
    if (theText[theCursor.first][index] == toFind) {
      setCursor(theCursor.first, index);
      return;
    }
  }
}

void Cursor::handleF(int toFind) {
  lastFind.first = 'F';
  lastFind.second = toFind;
  int index;
  for (index = theCursor.second - 1; index >= 0; --index) {
    if (theText[theCursor.first][index] == toFind) {
      setCursor(theCursor.first, index);
      return;
    }
  }
}

void Cursor::handleSemiColon() {
  if (lastFind.first == 'f') {
    handlef(lastFind.second);
  } else if (lastFind.first == 'F') {
    handleF(lastFind.second);
  }
}

void Cursor::updateStateOffset(int offset) { stateOffset = offset; }

void Cursor::handleCtrlD() {
  if (theText.size() == 1) return;

  int toMoveRow = std::min(theCursor.first + winSize.first / 2,
                           ifNegativeThenZero(theText.size() - 1));
  int col = 0;
  if (winSize.first >= theText.size()) {  // prevent seg fault
    while ((theText[0][col] == ' ' || theText[0][col] == '\t') &&
           col < theText[0].length())
      ++col;
    setCursor(0, col);
  } else if (winSize.first / 2 < 6 && theCursor.first < winSize.first / 2) {
    int offset = ifNegativeThenZero(winSize.first - 1);
    while ((theText[offset][col] == ' ' || theText[offset][col] == '\t') &&
           col < theText[offset].length())
      ++col;
    setCursor(offset, col);
  } else if (theCursor.first < winSize.first / 2) {
    while ((theText[(winSize.first) / 2 + 5][col] == ' ' ||
            theText[(winSize.first) / 2 + 5][col] == '\t') &&
           col < theText[(winSize.first) / 2 + 5].length())
      ++col;
    setCursor((winSize.first) / 2 + 5, col);
  } else {
    while (
        (theText[toMoveRow][col] == ' ' || theText[toMoveRow][col] == '\t') &&
        col < theText[toMoveRow].length())
      ++col;
    setCursor(toMoveRow, col);
  }
  if (toMoveRow != ifNegativeThenZero(theText.size() - 1)) {
    winPtr.first = theCursor.first - 6;
    winPtr.second = std::min(winPtr.first + winSize.first,
                             ifNegativeThenZero(theText.size() - 1));
  }
}

void Cursor::handleCtrlB() {
  int toMoveRow;
  if (winSize.first / 2 > 5) {
    toMoveRow = winPtr.first - 4;
  } else {
    toMoveRow = winPtr.first - std::floor(winSize.first / 2) + 1;
  }
  int offset = winSize.first / 2 > 5 ? 5 : winSize.first / 2;

  if (toMoveRow > winSize.first / 2) {
    int col = 0;
    while (
        (theText[toMoveRow][col] == ' ' || theText[toMoveRow][col] == '\t') &&
        col < theText[toMoveRow].length())
      ++col;
    setCursor(toMoveRow, col);
    winPtr.second = theCursor.first + offset;
    winPtr.first = winPtr.second - winSize.first;
  } else if (winPtr.first != 0) {
    int col = 0;
    while ((theText[winPtr.first + 1][col] == ' ' ||
            theText[winPtr.first + 1][col] == '\t') &&
           col < theText[winPtr.first + 1].length())
      ++col;
    setCursor(winPtr.first + 1, col);

    winPtr.first = 0;
    winPtr.second = winSize.first - 1;
  }
}

void Cursor::handleCtrlU() {
  int toMoveRow = ifNegativeThenZero(theCursor.first - winSize.first / 2);
  int col = 0;

  if (theCursor.first > winSize.first / 2) {
    int offset = ifNegativeThenZero(winPtr.first + (winSize.first) / 2 - 5);
    while ((theText[offset][col] == ' ' || theText[offset][col] == '\t') &&
           col < theText[offset].length())
      ++col;
    setCursor(offset, col);
  } else {
    while (
        (theText[toMoveRow][col] == ' ' || theText[toMoveRow][col] == '\t') &&
        col < theText[toMoveRow].length())
      ++col;
    setCursor(toMoveRow, col);
  }
  if (toMoveRow != 0) {
    winPtr.second = theCursor.first + 5;
    winPtr.first = ifNegativeThenZero(winPtr.second - winSize.first);
  }
}

void Cursor::handleCtrlF() {
  int toMoveRow;
  if (winSize.first / 2 > 5) {
    toMoveRow = winPtr.second + 4;
  } else {
    toMoveRow = winPtr.second + std::floor(winSize.first / 2) + 1;
  }
  if (toMoveRow < theText.size()) {
    int col = 0;
    while (
        (theText[toMoveRow][col] == ' ' || theText[toMoveRow][col] == '\t') &&
        col < theText[toMoveRow].length())
      ++col;
    setCursor(toMoveRow, col);

    winPtr.first = toMoveRow - 5;
    winPtr.second = winPtr.first + winSize.first;
  } else {
    int index = ifNegativeThenZero(theText.size() - 1);
    int col = 0;
    while ((theText[index][col] == ' ' || theText[index][col] == '\t') &&
           col < theText[index].length())
      ++col;
    setCursor(index, col);
  }
}

void Cursor::handleJ() {
  // does nothing on last row (or if only one row)
  if (theCursor.first == ifNegativeThenZero(theText.size() - 1)) return;

  // always 1 behind last letter of current row
  setCursor(theCursor.first, theText[theCursor.first].length());
  if (theText[theCursor.first + 1].length()) {
    while (theText[theCursor.first + 1][0] == ' ')
      theText[theCursor.first + 1].erase(0, 1);
    string middleSpace = theText[theCursor.first].back() == ' ' ? "" : " ";
    theText[theCursor.first] =
        theText[theCursor.first] + middleSpace + theText[theCursor.first + 1];
    theText.erase(theText.begin() + theCursor.first + 1);
  } else {  // empty line adds a space
    theText[theCursor.first] = theText[theCursor.first] + " ";
    theText.erase(theText.begin() + theCursor.first + 1);
  }
}

char Cursor::handlex() {
  if (!theText[theCursor.first].length()) return 'z';
  if (theText[theCursor.first].length() == 1) {
    char temp = theText[theCursor.first][0];
    theText[theCursor.first] = "";
    return temp;
  } else if (theCursor.second != theText[theCursor.first].length() - 1) {
    char temp = theText[theCursor.first][theCursor.second];
    theText[theCursor.first] =
        theText[theCursor.first].erase(theCursor.second, 1);
    return temp;
  } else {  // on the last character of the rows
    char temp = theText[theCursor.first][theCursor.second];
    theText[theCursor.first] =
        theText[theCursor.first].erase(theCursor.second, 1);
    setCursor(theCursor.first, theCursor.second - 1);
    return temp;
  }
}

// will add special case later
void Cursor::handlep(pair<vector<string>, bool>& clipBoard) {
  if (!clipBoard.first.size()) return;

  string firstChunk;
  string secondChunk;
  if (theText[theCursor.first].length()) {
    firstChunk = theText[theCursor.first].substr(0, theCursor.second + 1);
    secondChunk = theText[theCursor.first].substr(
        theCursor.second + 1, theText[theCursor.first].length());
  }
  if (clipBoard.second) {  // special case
    for (size_t i = 0; i < clipBoard.first.size(); ++i)
      theText.insert(theText.begin() + theCursor.first + 1 + i,
                     clipBoard.first[i]);
    nextLine();
    theCursor.second = 0;
  } else if (clipBoard.first.size() == 1) {
    theText[theCursor.first] = firstChunk + clipBoard.first[0] + secondChunk;
    ++theCursor.second;
  } else {
    theText[theCursor.first] = firstChunk + clipBoard.first[0];
    for (size_t i = 1; i < clipBoard.first.size() - 1; ++i) {
      theText.insert(theText.begin() + theCursor.first + i, clipBoard.first[i]);
    }
    theText.insert(
        theText.begin() + theCursor.first + clipBoard.first.size() - 1,
        clipBoard.first.back() + secondChunk);
  }
}

void Cursor::handleP(pair<vector<string>, bool>& clipBoard) {
  if (!clipBoard.first.size()) return;

  string firstChunk;
  string secondChunk;
  if (theText[theCursor.first].length()) {
    firstChunk = theText[theCursor.first].substr(0, theCursor.second);
    secondChunk = theText[theCursor.first].substr(
        theCursor.second, theText[theCursor.first].length());
  }

  if (clipBoard.second) {  // special case
    for (size_t i = 0; i < clipBoard.first.size(); ++i)
      theText.insert(theText.begin() + theCursor.first + i, clipBoard.first[i]);
    theCursor.second = 0;
  } else if (clipBoard.first.size() == 1) {
    theText[theCursor.first] = firstChunk + clipBoard.first[0] + secondChunk;
    ++theCursor.second;
  } else {
    theText[theCursor.first] = firstChunk + clipBoard.first[0];
    for (size_t i = 1; i < clipBoard.first.size() - 1; ++i) {
      theText.insert(theText.begin() + theCursor.first + i, clipBoard.first[i]);
    }
    theText.insert(
        theText.begin() + theCursor.first + clipBoard.first.size() - 1,
        clipBoard.first.back() + secondChunk);
  }
}

void Cursor::handleb() {
  // cursor is at the beginning
  if (!theCursor.first && !theCursor.second) return;
  int row = theCursor.first;
  int col = theCursor.second;

  string temp;
  int offset;
  // not the first row
  if (theCursor.first) {
    temp = theText[row - 1] + theText[row];
    offset = theText[row - 1].length();
  } else {
    temp = theText[row];
    offset = 0;
  }
  bool prevWord = isalnum(temp[col + offset]) || temp[col + offset] == '_';
  bool firstTime = true;
  bool space = false;
  int i;
  for (i = col + offset - 1; i >= 0; --i) {
    if (space && temp[i] != ' ') {
      bool partition = false;
      prevWord = isalnum(temp[i]) || temp[i] == '_';
      while (i >= 0) {
        if (i >= offset) {
          partition = true;
        }
        --i;
        if ((isalnum(temp[i]) || temp[i] == '_') != prevWord ||
            temp[i] == ' ' || (i < offset && partition)) {
          break;
        }
      }
      ++i;
      break;
    } else {
      // next row and not a space case
      if (i < offset && temp[i] != ' ') {
        if (firstTime) {
          prevWord = isalnum(temp[i]) || temp[i] == '_';
          while (i >= 0) {
            --i;
            if ((isalnum(temp[i]) || temp[i] == '_') != prevWord ||
                temp[i] == ' ') {
              break;
            }
          }
        }
        ++i;
        break;
      } else if (isalnum(temp[i]) || temp[i] == '_') {  // is a "word"
        if (!prevWord) {
          if (firstTime) {
            bool partition = false;
            prevWord = isalnum(temp[i]) || temp[i] == '_';
            while (i >= 0) {
              if (i >= offset) {
                partition = true;
              }
              --i;
              if ((isalnum(temp[i]) || temp[i] == '_') != prevWord ||
                  temp[i] == ' ' || (i < offset && partition)) {
                break;
              }
            }
          }
          ++i;
          break;
        }
        prevWord = true;
      } else {
        if (prevWord && temp[i] != ' ' && !space) {
          if (firstTime) {
            bool partition = false;
            prevWord = isalnum(temp[i]) || temp[i] == '_';
            while (i > 0) {
              if (i >= offset) {
                partition = true;
              }
              --i;
              if ((isalnum(temp[i]) || temp[i] == '_') != prevWord ||
                  temp[i] == ' ' || (i < offset && partition)) {
                break;
              }
            }
          }
          ++i;
          break;
        } else if (!space && temp[i] == ' ') {
          // hit a space and it's not the first time
          if (!firstTime) {
            ++i;
            break;
          }
          space = true;
        }
        prevWord = false;
      }
    }
    firstTime = false;
  }
  if (i == theCursor.second && !theCursor.first) {
    setCursor(0, 0);
    return;
  }
  if (theCursor.first && i >= theText[row - 1].length()) {  // stay on same row
    if (!theText[row - 1].length() && i == -1) {
      setCursor(row - 1, 0);
    } else {
      int newCol = i - theText[row - 1].length();
      setCursor(row, newCol);
    }
  } else if (!theCursor.first) {
    setCursor(row, ifNegativeThenZero(i));
  } else {
    --row;
    setCursor(row, ifNegativeThenZero(i));
  }
}

void Cursor::handler(int input) {
  if (!theText[theCursor.first].length()) return;
  theText[theCursor.first][theCursor.second] = input;
}

void Cursor::handlew() {
  // cursor is already at the end
  if (theCursor.first == ifNegativeThenZero(theText.size() - 1) &&
      theCursor.second == ifNegativeThenZero(theText.back().length() - 1))
    return;
  if (theText[theCursor.first].length() == 0 &&
      theText.size() >= theCursor.first) {
    int col = 0;
    while (col < theText[theCursor.first + 1].length() &&
           theText[theCursor.first + 1][col] == ' ')
      ++col;
    setCursor(theCursor.first + 1, col);
    return;
  }
  int row = theCursor.first;
  int col = theCursor.second;
  string temp;
  if (theCursor.first != ifNegativeThenZero(theText.size() - 1)) {
    temp = theText[row] + theText[row + 1];
  } else {
    temp = theText[row];
  }
  bool prevWord = false;
  bool firstTime = true;
  bool space = false;

  int i;
  for (i = col; i < temp.length(); ++i) {
    if (i > ifNegativeThenZero(theText[row].length() - 1) && temp[i] != ' ') {
      break;
    }
    if (space && temp[i] != ' ') {
      break;
    } else {
      if (isalnum(temp[i]) || temp[i] == '_') {
        if (!prevWord && !firstTime) {
          break;
        }
        prevWord = true;
      } else {
        if (prevWord && temp[i] != ' ' && !firstTime) {
          break;
        }
        if (temp[i] == ' ') space = true;
        prevWord = false;
      }
    }
    firstTime = false;
  }

  if (i >= theText[row].length()) {
    int newCol = i - theText[row].length();
    if (row == theText.size() - 1) {
      while (theText[row][i] == ' ' && i < theText[row].length()) {
        ++i;
      }
      setCursor(row, i - 1);
      return;
    } else if (theText[row + 1][newCol] == ' ') {
      while (theText[row + 1][newCol] == ' ' && i < theText[row + 1].length()) {
        ++newCol;
      }
    }
    setCursor(row + 1, newCol);
  } else {
    setCursor(row, i);
  }
}
}  // namespace CS246E
