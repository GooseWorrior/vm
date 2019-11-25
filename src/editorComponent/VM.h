#ifndef VM_H
#define VM_H

#include <string>
#include <vector>

#include "../Model.h"
#include "../view/PlainView.h"

#include "Commandline.h"
//#include "Cursor.h"
#include "EditorComponent.h"
#include "StatusLine.h"

using std::pair;
using std::string;
using std::unique_ptr;
using std::vector;

namespace CS246E {
class VM : public Model {
  int state;  // 0 - command/readonly, 1 - insert, 2 - commandline
  Cursor vcursor;
  EditorComponent theComponents;
  pair<int, int> WindowSize;
  pair<int, int> WindowPointer;
  vector<string> text;
  vector<unique_ptr<EditorComponent>> components;
  vector<string> undoStack;
  vector<pair<int, int>> cursorStack;
  string vmStatusString;

  int checkLineLength(int x, int lineLength);
  void findPairedBracket();
  void handleCommands(int input);
  void loadFile(string filename);
  void saveText();
  void loadUndo();
  void loadCursor();

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
  void printPlaceholder();
  friend class PlainView;
};
}  // namespace CS246E
#endif
