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
  virtual void update() = 0;      // stays pure virtual
  virtual void initialize() = 0;  // stays pure virtual

  void display(pair<int, int> prevPointer, int input, pair<int, int> prevCursor,
               pair<int, int> prevWindowSize, int prevChar, int prevSize,
               bool edit);
  void printTextAll();
  void printPlaceholder();
  void printTextAfterward(int input, pair<int, int> prevCursor);
  void printTextChar(int input, int prevChar);
  void printTextLine(int input, pair<int, int> prevCursor, int prevChar);
  virtual ~View();
};
}  // namespace CS246E
#endif
