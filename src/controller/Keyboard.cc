#include "Keyboard.h"
#include <ncurses.h>

namespace CS246E {
int Keyboard::getChar() { return getch(); }
Keyboard::Keyboard() {}
}  // namespace CS246E
