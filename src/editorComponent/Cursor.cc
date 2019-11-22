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
}  // namespace CS246E
