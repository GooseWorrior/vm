#include <ncurses.h>
#include <iostream>
#include <memory>
#include <string>

#include "controller/Keyboard.h"
#include "editorComponent/VM.h"
#include "view/PlainView.h"

using std::shared_ptr;
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
  keypad(stdscr, true);  // fech all
  cbreak();              // fetch all
  set_escdelay(0);

  start_color();  // enable color
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  init_pair(2, COLOR_WHITE, COLOR_RED);

  init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(4, COLOR_GREEN, COLOR_BLACK);
  init_pair(5, COLOR_YELLOW, COLOR_BLACK);
  init_pair(6, COLOR_RED, COLOR_BLACK);
  init_pair(7, COLOR_CYAN, COLOR_BLACK);
  init_pair(8, COLOR_WHITE, COLOR_CYAN);
  init_pair(9, COLOR_YELLOW, COLOR_BLACK);
  unique_ptr<CS246E::VM> model = std::make_unique<CS246E::VM>(filename);
  unique_ptr<CS246E::Keyboard> controller =
      std::make_unique<CS246E::Keyboard>();
  model->addController(std::move(controller));
  model->process();

  endwin();
  return 0;
}
