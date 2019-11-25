#ifndef CRUSH_H
#define CRUSH_H

#include <ctype.h>
#include <stdio.h>
#include <string>
#include <vector>

using std::pair;
using std::string;
using std::vector;
namespace CS246E {
class Cursor {
  int stateOffset;
  pair<int, int> theCursor;
  pair<char, char> lastFind;  // first is f/F, second is any char
  vector<string>& theText;

  // helpers for handlePercentage
  void moveToCloseBracket(char openBracket, char closeBracket);
  void moveToOpenBracket(char openBracket, char closeBracket);
  void findPairedBracket();
  int checkClosest(vector<pair<int, int>>& stack, int& closest, int i, int j);

  pair<int, int>& winPtr;
  pair<int, int>& winSize;
  int& state;

 public:
  Cursor(int row, int col, vector<string>& theText, pair<int, int>& winPtr,
         pair<int, int>& winSize, int& state);
  Cursor& operator++();
  Cursor& operator--();
  Cursor& nextLine();
  Cursor& prevLine();
  Cursor& insert(wchar_t c);
  int erase();
  int getRow();
  int getCol();
  void setCursor(int x, int y);
  void handlePercentage(char input);
  void handleCaret();
  void handlef(int toFind);
  void handleF(int toFind);
  void handleSemiColon();
  void handleb();
  void updatePointer(int mode);
  int calculateLine();
  void updateStateOffset(int offset);
};
}  // namespace CS246E
#endif
