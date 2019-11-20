#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
using std::string;
namespace CS246E {
class Controller {
 public:
  Controller();
  virtual string getChar() = 0;
};
}  // namespace CS246E

#endif
