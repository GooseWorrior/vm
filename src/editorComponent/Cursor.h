#ifndef CRUSH_H
#define CRUSH_H

#include <ctype.h>
#include <stdio.h>
#include <memory>
#include <string>
#include <vector>

using std::pair;
using std::string;
using std::unique_ptr;
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
  // unique_ptr<ControlCommand> ctrlcmd;

 public:
  vector<pair<int, int>> replaceModeDelete;
  vector<int> dot;

  Cursor(int row, int col, vector<string>& theText, pair<int, int>& winPtr,
         pair<int, int>& winSize, int& state);
  Cursor& operator++();
  Cursor& operator--();
  Cursor& nextLine();
  Cursor& prevLine();
  Cursor& insert(int c, int pseudoState = -1);
  int erase(int prevInput, int input, int pseudoState = -1);
  int getRow();
  int getCol();
  void setCursor(int x, int y);
  void handlePercentage(char input);
  void handleCaret();
  void handlef(int toFind);
  void handleF(int toFind);
  void handleSemiColon();
  void handleb();
  void handlew();
  void handleCtrlD();
  void handleCtrlB();
  void handleCtrlU();
  void handleCtrlF();
  void handleJ();
  char handlex();
  int handleDot(pair<int, int> lastCommand);
  void handlep(pair<vector<string>, bool>& clipBoard);
  void handleP(pair<vector<string>, bool>& clipBoard);
  void handler(int input);
  void updatePointer(int mode);
  bool canDelete();
  int calculateLine();
  void updateStateOffset(int offset);
};
}  // namespace CS246E
#endif
