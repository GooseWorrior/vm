#ifndef VM_H
#define VM_H

#include <string>
#include <vector>

#include "../Model.h"

#include "Commandline.h"
#include "Cursor.h"
#include "EditorComponent.h"
#include "StatusLine.h"

using std::pair;
using std::string;
using std::unique_ptr;
using std::vector;

namespace CS246E {
class VM : public Model {
  Cursor vcursor;
  pair<int, int> WindowSize;
  pair<int, int> WindowPointer;
  vector<string> text;
  vector<unique_ptr<EditorComponent>> components;

  int checkLineLength(int x, int lineLength);
  void findPairedBracket();

 public:
  VM(string filename);
  void process();
  bool updateWindowSize();
  pair<int, int> updateLoc();
  void printTextAll();  // temporary
  void printTextAfterward(int input, pair<int, int> prevCursor);
  void printTextLine(int input, pair<int, int> prevCursor,
                     int prevChar);  // temporary
  void printTextChar(int input, int prevChar);
};
}  // namespace CS246E
#endif
