#include <ncurses.h>
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
  string a = "fdsdsfdsfsfdfds\ndsfsdf\nsdfdssdfsdf\n\ndsfsdf";
  printw("%s", text.c_str());
  refresh();
}

}  // namespace CS246E
