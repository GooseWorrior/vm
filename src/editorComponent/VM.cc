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
  initscr();

  std::ifstream file{filename};
  file >> std::noskipws;

  char c;
  while (file >> c) {
    text += c;
  }
  string a = "fdsdsfdsfsfdfds\ndsfsdf\nsdfdssdfsdf\n\ndsfsdf";
  printw("%s", text.c_str());
  refresh();
  getch();
  endwin();
  std::cout << text;  // remove after debugging
}
}  // namespace CS246E
