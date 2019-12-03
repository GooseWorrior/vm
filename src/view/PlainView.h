#ifndef PLAIN_VIEW_H
#define PLAIN_VIEW_H

#include <memory>
#include <string>
#include <vector>

#include "View.h"

using std::shared_ptr;
using std::string;
using std::vector;
namespace CS246E {
class VM;
class PlainView : public View {
  void printTextLine(int input, pair<int, int> prevCursor,
                     int prevChar);  // temporary
  void printTextChar(int input, int prevChar);
  void printTextAfterward(int input, pair<int, int> prevCursor);

 public:
  PlainView(VM* vm);
  void update() override;
  ~PlainView();

  // transport functions
  void printTextAll();  // temporary
  void printPlaceholder();

  void initialize();
};
}  // namespace CS246E
#endif
