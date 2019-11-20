#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <string>

// #include "controller/Keyboard.h"
// #include "view/HighlightView.h"
// #include "view/PlainView.h"

using std::string;
using std::unique_ptr;

namespace CS246E {
// forward declaration
class View;
class Keyboard;

class Model {
 protected:
  unique_ptr<View> view;
  unique_ptr<Keyboard> controller;
  string filename;

 public:
  Model();
  ~Model();
  //  protected:
  void addController(unique_ptr<Keyboard> controller);
  void addView(unique_ptr<View> view);
};
}  // namespace CS246E
#endif
