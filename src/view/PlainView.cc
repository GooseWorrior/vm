#include "PlainView.h"

namespace CS246E {
PlainView::PlainView(shared_ptr<VM> vm) : View{} { this->vm = vm; }

void PlainView::display(pair<int, int> prevPointer, int input,
                        pair<int, int> prevCursor, int prevChar, int prevSize,
                        bool edit) {
  if (vm->updateWindowSize() ||
      (prevPointer.first != vm->WindowPointer.first &&
       prevPointer.second != vm->WindowPointer.second &&
       vm->WindowPointer.second - vm->WindowPointer.first + 1 <
           vm->text.size())) {
    vm->printTextAll();
  } else if (vm->text.size() != prevSize) {
    vm->printTextAfterward(input, prevCursor);
  } else if (edit &&
             vm->vcursor.getCol() != vm->text[vm->vcursor.getRow()].size()) {
    vm->printTextLine(input, prevCursor, prevChar);
  } else if (edit) {
    vm->printTextChar(input, prevChar);
  }
}
void PlainView::update() {}
PlainView::~PlainView() {}
}  // namespace CS246E
