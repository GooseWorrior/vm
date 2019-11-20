#include "Keyboard.h"
#include <ncurses.h>

namespace CS246E {
char Keyboard::getChar() {
  return getch();
  // return string(1, getch());
}
Keyboard::Keyboard() {}
}  // namespace CS246E
