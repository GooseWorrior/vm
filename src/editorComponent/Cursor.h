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
  vector<string>& theText;

 public:
  Cursor(int row, int col, vector<string>& theText);
  Cursor& operator++();
  Cursor& operator--();
  Cursor& nextLine();
  Cursor& prevLine();
  Cursor& insert(wchar_t c);
  int erase();
  int getRow();
  int getCol();
  void setCursor(int x, int y);
};
}  // namespace CS246E
#endif
