#ifndef MODEL_H
#define MODEL_H

#include <string>

#include "controller/Controller.h"
#include "view/View.h"

using std::string;

namespace CS246E {
class Model {
  View view;
  Controller controller;
  string filename;

 public:
  Model();
};
}  // namespace CS246E
#endif
