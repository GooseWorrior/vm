#ifndef VM_H
#define VM_H

#include <string>
#include <vector>

#include "../Model.h"

#include "Commandline.h"
#include "Cursor.h"
#include "EditorComponent.h"
#include "StatusLine.h"

using std::string;
using std::unique_ptr;
using std::vector;
using std::pair;

namespace CS246E {
class VM : public Model {
  Cursor vcursor;
  pair<int, int> WindowSize;
  vector<string> text;
  vector<unique_ptr<EditorComponent>> components;
 public:
  VM(string filename);
  void process();
  bool updateWindowSize();
  pair<int, int> updateLoc();
  void printTextAll(); // temporary
  void printTextAfterward(int input, pair<int, int> prevCursor);
  void printTextLine(int input, pair<int, int> prevCursor, int prevChar); // temporary
  void printTextChar(int input, int prevChar);
};
}  // namespace CS246E
#endif
