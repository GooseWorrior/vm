#include "Cursor.h"
#include <algorithm>
#include <ncurses.h>
using std::min;
using std::pair;
using std::string;
using std::vector;

namespace CS246E {
Cursor::Cursor(int row, int col, vector<string>& theText, pair<int, int> & winPtr, pair<int, int> & winSize)
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
  } else if ((theCursor.first - winPtr.first) >= (winPtr.second - winPtr.first) / 3 * 2 && winPtr.second < theText.size() - 1){
    winPtr.second++;
    updatePointer(-1);
    //int shift = calculateShift();
    //theCursor.first += calculateShift();
  } 
  theCursor.second =
  min<int>(theCursor.second, theText[++theCursor.first].size());
  return *this;
}
Cursor& Cursor::prevLine() {
  if (theCursor.first == 0) {
    return *this;
  } else if ((theCursor.first - winPtr.first) <=  (winPtr.second - winPtr.first) / 3 && winPtr.first > 0) {
    winPtr.first--;
    updatePointer(1);
    //int shift = calculateShift();
    //theCursor.first += calculateShift();
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
    } else if (calculateLine() < winSize.first){
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
    theCursor.second = theText[theCursor.first - 1].size();
    theText[theCursor.first - 1] += theText[theCursor.first];
    theText.erase(theText.begin() + theCursor.first);
    if ((theCursor.first - winPtr.first) <=  (winPtr.second - winPtr.first) / 3 && winPtr.first > 0) {
        winPtr.first--;
        updatePointer(1);
    } else if (calculateLine() < winSize.first) {
        updatePointer(1);
    }
    theCursor.first--;
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
                theCursor.first = min(winPtr.second, theCursor.first);
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
       if (winPtr.first > theCursor.first || winPtr.second + 1 < theCursor.first) {
           int offset = winPtr.second - winPtr.first;
           winPtr.second = min<int>((winSize.second - winSize.second) / 2 + theCursor.first, theText.size() - 1);
           winPtr.first = winPtr.second - offset;
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

}  // namespace CS246E
