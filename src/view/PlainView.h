#ifndef PLAIN_VIEW_H
#define PLAIN_VIEW_H

#include <memory>

#include "../editorComponent/VM.h"
#include "View.h"

using std::shared_ptr;
namespace CS246E {
class VM;
class PlainView : public View {
  VM* vm;

 public:
  PlainView(VM* vm);
  void update() override;
  void display(pair<int, int> prevPointer, int input, pair<int, int> prevCursor,
               int prevChar, int prevSize, bool edit) override;
  ~PlainView();
};
}  // namespace CS246E
#endif
