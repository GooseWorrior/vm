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
  while (file >> c) {
    text += c;
  }
  printw("%s", text.c_str());
  refresh();
}
void VM::process() {
  while (true) {
    char input = controller->getChar();
    if (input == 'q') {
      break;
    }
    text += input;
    addch(input);
    refresh();
  }
}
}  // namespace CS246E
