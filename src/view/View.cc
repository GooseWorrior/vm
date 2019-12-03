#include "View.h"
#include "../editorComponent/VM.h"

namespace CS246E {
void View::display(pair<int, int> prevPointer, int input,
                   pair<int, int> prevCursor, pair<int, int> prevWindowSize,
                   int prevChar, int prevSize, bool edit) {
  if (prevWindowSize != vm->WindowSize ||
      prevPointer.first < vm->WindowPointer.first ||
      prevPointer.second > vm->WindowPointer.second || input == 'u') {
    printTextAll();
  } else if (vm->text.size() != prevSize) {
    printTextAfterward(input, prevCursor);
  } else if (edit &&
             vm->vcursor.getCol() != vm->text[vm->vcursor.getRow()].size()) {
    printTextLine(input, prevCursor, prevChar);
  } else if (edit) {
    printTextChar(input, prevChar);
  }

  if (vm->vcursor.calculateLine() < vm->WindowSize.first &&
          prevPointer != vm->WindowPointer ||
      prevWindowSize != vm->WindowSize || input == 'u') {
    printPlaceholder();
  }
  update();
}
View::View(VM* vm) : vm{vm} {}
View::~View() {}
}  // namespace CS246E