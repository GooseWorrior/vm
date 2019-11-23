#ifndef CRUSH_H
#define CRUSH_H

#include <string>
#include <vector>
#include "EditorComponent.h"

using std::pair;
using std::string;
using std::vector;
namespace CS246E {
class Cursor {
  pair<int, int> theCursor;
  pair<char, char> lastFind;  // first is f/F, second is any char
  vector<string>& theText;

  // helpers for handlePercentage
  void moveToCloseBracket(char openBracket, char closeBracket);
  void moveToOpenBracket(char openBracket, char closeBracket);
  void findPairedBracket();

  pair<int, int> & winPtr;
  pair<int, int> & winSize;
 public:
  Cursor(int row, int col, vector<string>& theText, pair<int, int> & winPtr, pair<int, int> & winSize);
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
  private:
  void updatePointer(int mode);
};
}  // namespace CS246E
#endif
