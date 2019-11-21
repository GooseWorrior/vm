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
      line = "";
    }
    addch(c);
  }
  this->text.push_back(line);
  refresh();
}
void VM::process() {
  int x = text.back().length();
  int y = text.size() - 1;
  // for (auto i = text.begin(); i != text.end(); ++i) std::cout << *i << ' ';

  int input;
  while (input != 'q') {
    move(y, x);
    // printw("%d, %d", x, y);
    input = controller->getChar();
    switch (input) {
      case KEY_LEFT:
        if (x > 0) {
          --x;
        }
        break;
      case KEY_RIGHT:
        if (x < text[y].length() - 1 ||
            (y == text.size() - 1 && x < text[y].length())) {
          ++x;
        }
        break;
      case KEY_UP:
        if (y > 0) {
          --y;
        }
        break;
      case KEY_DOWN:
        if (y < text.size() - 1) {
          ++y;
        }
        break;
      default:
        text.back() += input;
        printw("%c", input);
        if (input == '\n') {
          text.push_back("");
          ++y;
          x = -1;
        }
        ++x;
        break;
    }
    refresh();
  }
}
}  // namespace CS246E
