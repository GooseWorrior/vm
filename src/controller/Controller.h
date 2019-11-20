#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
using std::string;
namespace CS246E {
class Controller {
 public:
  Controller();
  virtual char getChar() = 0;
};
}  // namespace CS246E

#endif
