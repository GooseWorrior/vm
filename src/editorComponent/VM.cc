#include <curses.h>
#include <fstream>
#include <iostream>  // remove after debugging

#include "../Model.h"
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

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  std::cout << text;  // remove after debugging
}
}  // namespace CS246E
