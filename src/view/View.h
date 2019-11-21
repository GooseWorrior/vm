#ifndef VIEW_H
#define VIEW_H

namespace CS246E {
class View {
 public:
  View();
  virtual void update() = 0;
  virtual void display() = 0;
  virtual ~View();
};
}  // namespace CS246E
#endif
