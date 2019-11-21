#include <ncurses.h>
#include <iostream>
#include <memory>
#include <string>

#include "EditorComponent/VM.h"
#include "controller/Keyboard.h"

using std::string;
using std::unique_ptr;

int main(int argc, char *argv[]) {
  // might change later if doing multiple files
  if (argc != 2) {
    return 1;
  }
  string filename = argv[1];

  initscr();
  noecho();
  keypad(stdscr, TRUE);
  // cbreak();

  unique_ptr<CS246E::VM> model = std::make_unique<CS246E::VM>(filename);
  unique_ptr<CS246E::Keyboard> controller =
      std::make_unique<CS246E::Keyboard>();
  model->addController(std::move(controller));
  model->process();

  endwin();  // remove after quit command is written
  return 0;
}
