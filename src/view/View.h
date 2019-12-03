#ifndef VIEW_H
#define VIEW_H

#include <utility>

using std::pair;
namespace CS246E {
class VM;
class View {
 protected:
  VM* vm;

 public:
  View(VM* vm);
  virtual void update() = 0;
  void display(pair<int, int> prevPointer, int input, pair<int, int> prevCursor,
               pair<int, int> prevWindowSize, int prevChar, int prevSize,
               bool edit);
  virtual void printTextAll() = 0;      // fix abstract problem
  virtual void printPlaceholder() = 0;  // fix abstract problem
  virtual void initialize() = 0;
  virtual void printTextAfterward(int input, pair<int, int> prevCursor) = 0;
  virtual void printTextChar(int input, int prevChar) = 0;
  virtual void printTextLine(int input, pair<int, int> prevCursor,
                             int prevChar) = 0;
  virtual ~View();
};
}  // namespace CS246E
#endif
