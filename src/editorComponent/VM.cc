#include <ncurses.h>
#include <fstream>
#include <iostream>  // remove after debugging

#include "../Model.h"
#include "../controller/Keyboard.h"
#include "VM.h"

#include "Commandline.h"
#include "Cursor.h"
#include "EditorComponent.h"
#include "StatusLine.h"

namespace CS246E {
VM::VM(string filename) {
  // load files
  std::ifstream file{filename};
  file >> std::noskipws;

  char c;
  string line = "";
  while (file >> c) {
    line += c;
    if (c == '\n') {
      this->text.push_back(line);
    }
    addch(c);
  }
  refresh();
}
void VM::process() {
  int x = text.size();
  int y = text.back().length();
  int input;
  while (input != 'q') {
    move(y, x);
    input = controller->getChar();
    switch (input) {
      case KEY_LEFT:
        x--;
        break;
      case KEY_RIGHT:
        x++;
        break;
      case KEY_UP:
        y--;
        break;
      case KEY_DOWN:
        y++;
        break;
      default:
        text.back() += input;
        if (input == '\n') {
          text.push_back("");
        }
        printw("%c", input);
        x++;
        break;
    }
    refresh();
  }
}
}  // namespace CS246E
