#include "Keyboard.h"
#include <ncurses.h>

namespace CS246E {
string Keyboard::getChar() { return string(1, getch()); }
Keyboard::Keyboard() {}
}  // namespace CS246E
