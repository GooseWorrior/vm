#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <curses.h>
#include "Controller.h"

namespace CS246E {
class Keyboard : public Controller {
 public:
  Keyboard();
  int getChar() override;
};
}  // namespace CS246E
#endif
