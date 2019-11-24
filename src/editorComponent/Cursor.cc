#include "Cursor.h"
#include <ncurses.h>
#include <algorithm>
using std::min;
using std::pair;
using std::string;
using std::vector;

namespace CS246E {
Cursor::Cursor(int row, int col, vector<string>& theText,
               pair<int, int>& winPtr, pair<int, int>& winSize)
    : theCursor{row, col}, theText{theText}, winPtr{winPtr}, winSize{winSize} {}
Cursor& Cursor::operator++() {
  if (theCursor.second < theText[theCursor.first].size()) {
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
  theCursor.second =
      min<int>(theCursor.second, theText[++theCursor.first].size());
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
  theCursor.second =
      min<int>(theCursor.second, theText[--theCursor.first].size());
  return *this;
}
int Cursor::getRow() { return theCursor.first; }
int Cursor::getCol() { return theCursor.second; }
Cursor& Cursor::insert(wchar_t c) {
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
  } else {
    theText[theCursor.first].insert(theCursor.second, 1, c);
    ++(*this);
  }
  return *this;
}
int Cursor::erase() {
  char prevChar = 0;
  if (theCursor.second == 0 && theCursor.first == 0) {
    return prevChar;
  } else if (theCursor.second == 0) {
    theText[theCursor.first - 1] += theText[theCursor.first];
    theText.erase(theText.begin() + theCursor.first);
    if ((theCursor.first - winPtr.first) <=
            (winPtr.second - winPtr.first)  &&
        winPtr.first > 0) {
      winPtr.first--;
      updatePointer(1);
    } else if (calculateLine() < winSize.first) {
      updatePointer(1);
    }
    theCursor.first--;
    theCursor.second = theText[theCursor.first].size();
  } else {
    prevChar = theText[theCursor.first][theCursor.second - 1];
    theText[theCursor.first].erase(theCursor.second - 1, 1);
    --(*this);
  }
  return prevChar;
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
    winPtr.second = 0;
  } else if (mode == 0) {
    if (winPtr.first > theCursor.first || winPtr.second < theCursor.first) {
      int offset = winPtr.second - winPtr.first;
      winPtr.second =
          min<int>((winSize.second - winSize.second) / 2 + theCursor.first,
                   theText.size() - 1);
      winPtr.first = winPtr.second - offset;
    }
    //theCursor.first = min(winPtr.second, theCursor.first);
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
  for (size_t i = 0; i < theText.size(); ++i) {
    for (size_t j = 0; j < theText[i].length(); ++j) {
      if (theText[i][j] == '[') {
        squareStack.push_back(std::make_pair(i, j));
      } else if (theText[i][j] == '(') {
        roundStack.push_back(std::make_pair(i, j));
      } else if (theText[i][j] == '{') {
        curlyStack.push_back(std::make_pair(i, j));
      } else if (theText[i][j] == ']') {
        checkClosest(squareStack, closest, i, j);
      } else if (theText[i][j] == ')') {
        checkClosest(roundStack, closest, i, j);
      } else if (theText[i][j] == '}') {
        checkClosest(curlyStack, closest, i, j);
      }
    }
  }
  if (closest != theText[theCursor.first].length() + 1) {
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
  lastFind.first = 'F';
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
}  // namespace CS246E
