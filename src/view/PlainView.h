#ifndef PLAIN_VIEW_H
#define PLAIN_VIEW_H

#include "View.h"
namespace CS246E {
class VM;
class PlainView : public View {
 public:
  PlainView(VM* vm);
  void update() override;
  void initialize();
  ~PlainView();
};
}  // namespace CS246E
#endif
