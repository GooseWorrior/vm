#include "Cursor.h"
#include <algorithm>

using std::min;
using std::pair;
using std::string;
using std::vector;

namespace CS246E {
Cursor::Cursor(int row, int col, vector<string>& theText)
    : theCursor{row, col}, theText{theText} {}
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
  } else {
    theCursor.second =
        min<int>(theCursor.second, theText[++theCursor.first].size());
  }
  return *this;
}
Cursor& Cursor::prevLine() {
  if (theCursor.first == 0) {
    return *this;
  } else {
    theCursor.second =
        min<int>(theCursor.second, theText[--theCursor.first].size());
  }
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
    theCursor.second = theText[theCursor.first - 1].size();
    theText[theCursor.first - 1] += theText[theCursor.first];
    theText.erase(theText.begin() + theCursor.first);
    theCursor.first--;
  } else {
    prevChar = theText[theCursor.first][theCursor.second - 1];
    theText[theCursor.first].erase(theCursor.second - 1, 1);
    --(*this);
  }
  return prevChar;
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
        if (squareStack.size()) {
          if (squareStack.back().second > theCursor.second &&
              squareStack.back().first == theCursor.first &&
              closest - theCursor.second >
                  squareStack.back().second - theCursor.second) {
            closest = squareStack.back().second;
          }
          if (j > theCursor.second && i == theCursor.first &&
              closest - theCursor.second > j - theCursor.second) {
            closest = j;
          }
          squareStack.pop_back();
        }
      } else if (theText[i][j] == ')') {
        if (roundStack.size()) {
          if (roundStack.back().second > theCursor.second &&
              roundStack.back().first == theCursor.first &&
              closest - theCursor.second >
                  roundStack.back().second - theCursor.second) {
            closest = roundStack.back().second;
          }
          if (j > theCursor.second && i == theCursor.first &&
              closest - theCursor.second > j - theCursor.second) {
            closest = j;
          }
          roundStack.pop_back();
        }
      } else if (theText[i][j] == '}') {
        if (curlyStack.size()) {
          if (curlyStack.back().second > theCursor.second &&
              curlyStack.back().first == theCursor.first &&
              closest - theCursor.second >
                  curlyStack.back().second - theCursor.second) {
            closest = curlyStack.back().second;
          }
          if (j > theCursor.second && i == theCursor.first &&
              closest - theCursor.second > j - theCursor.second) {
            closest = j;
          }
          curlyStack.pop_back();
        }
      }
    }
  }
  if (closest != theText[theCursor.first].length() + 1) {
    setCursor(theCursor.first, closest);
  }
}
}  // namespace CS246E
