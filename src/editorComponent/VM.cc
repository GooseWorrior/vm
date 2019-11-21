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
  // refresh();
}

int VM::checkLineLength(int x, int lineLength) {
  return lineLength > x ? x : lineLength == 0 ? lineLength : lineLength - 1;
}

void VM::process() {
  int x = text.back().length();
  int y = text.size() - 1;
  // for (auto i = text.begin(); i != text.end(); ++i) std::cout << *i << ' ';

  int input;
  while (input != 'q') {
    wmove(stdscr, y, x);
    mvcur(50, 50, y, x);
    // printw("yeet");

    // printw("%d, %d", x, y);
    input = controller->getChar();
    switch (input) {
      case KEY_LEFT:
        if (x > 0) {
          --x;
        }
        break;
      case KEY_RIGHT:
        if (x < static_cast<int>(text[y].length()) - 1 ||
            (y == text.size() - 1 && x < text[y].length())) {
          // printw("[%d,%d, %d]", x, text[y].length() - 1,
          //        x < text[y].length() - 1);
          ++x;
        }
        break;
      case KEY_UP:
        if (y > 0) {
          --y;
          x = checkLineLength(x, text[y].length());
        }
        break;
      case KEY_DOWN:
        if (y < text.size() - 1) {
          ++y;
          x = checkLineLength(x, text[y].length());
        }
        break;
      default:
        if (input == '\n') {
          if (y == static_cast<int>(text.size()) - 1) {
            text.push_back("");
          } else {
            // TODO
            text[y] = text[y].substr(0, x);
          }
          ++y;
          x = -1;
        } else {
          text[y] = text[y].substr(0, x) + string(1, input) +
                    text[y].substr(x, text[y].length());
          if (x == text[y].length() - 1) {
            addch(input);
          } else {
            // insch(input);
            mvprintw(y, 0, "%s", text[y].c_str());
          }
        }
        ++x;

        // refresh();
        break;
    }
  }
}
}  // namespace CS246E
