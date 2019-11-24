#ifndef VIEW_H
#define VIEW_H

#include <utility>

using std::pair;

namespace CS246E {
class View {
 public:
  View();
  virtual void update() = 0;
  virtual void display(pair<int, int> prevPointer, int input,
                       pair<int, int> prevCursor, int prevChar, int prevSize,
                       bool edit) = 0;
  virtual ~View();
};
}  // namespace CS246E
#endif
