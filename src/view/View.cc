#include "View.h"
#include <ncurses.h>
#include "../editorComponent/VM.h"

namespace CS246E {
View::View(VM* vm) : vm{vm} {}
View::~View() {}

void View::display(pair<int, int> prevPointer, int input,
                   pair<int, int> prevCursor, pair<int, int> prevWindowSize,
                   int prevChar, int prevSize, bool edit) {
  if (prevWindowSize != vm->WindowSize ||
      prevPointer.first < vm->WindowPointer.first ||
      prevPointer.second > vm->WindowPointer.second || input == 'u') {
    printTextAll();
  } else if (vm->text.size() != prevSize) {
    printTextAfterward(input, prevCursor);
  } else if (edit &&
             vm->vcursor.getCol() != vm->text[vm->vcursor.getRow()].size()) {
    printTextLine(input, prevCursor, prevChar);
  } else if (edit) {
    printTextChar(input, prevChar);
  }

  if (vm->vcursor.calculateLine() < vm->WindowSize.first &&
          prevPointer != vm->WindowPointer ||
      prevWindowSize != vm->WindowSize || input == 'u') {
    printPlaceholder();
  }
  update();
}

void View::printTextAll() {
  clear();
  for (size_t i = vm->WindowPointer.first; i <= vm->WindowPointer.second; ++i) {
    for (int j = 0; j < vm->text[i].size(); ++j) {
      if (vm->text[i][j] == '\t') {
        addstr("        ");
      } else {
        addch(vm->text[i][j]);
      }
    }
    addch('\n');
  }
  refresh();
}

void View::printPlaceholder() {
  attron(COLOR_PAIR(1));
  string placeholderString = "";
  for (int i = vm->vcursor.calculateLine(); i < vm->WindowSize.first - 1; i++) {
    placeholderString += "~\n";
  }
  placeholderString += "~";
  printw("%s", placeholderString.c_str());
  refresh();
  attroff(COLOR_PAIR(1));
}

void View::printTextAfterward(int input, pair<int, int> prevCursor) {
  clrtobot();
  refresh();
  pair<int, int> loc = vm->updateLoc();
  move(loc.first, 0);
  for (size_t i = vm->vcursor.getRow(); i <= vm->WindowPointer.second; ++i) {
    clrtoeol();
    for (int j = 0; j < vm->text[i].size(); ++j) {
      if (vm->text[i][j] == '\t') {
        addstr("        ");
      } else {
        addch(vm->text[i][j]);
      }
    }
    addch('\n');
    refresh();
  }
}

void View::printTextLine(int input, pair<int, int> prevCursor, int prevChar) {
  if ((input == KEY_BACKSPACE) && prevChar == 0) return;
  pair<int, int> loc = vm->updateLoc();
  if (input == KEY_BACKSPACE) {
    clrtoeol();
    move(loc.first, loc.second);
    for (size_t i = vm->vcursor.getCol();
         i < vm->text[vm->vcursor.getRow()].size(); ++i) {
      if (vm->text[vm->vcursor.getRow()][i] == '\t') {
        addstr("        ");
      } else {
        addch(vm->text[vm->vcursor.getRow()][i]);
      }
    }
  } else if (vm->state == 1) {
    if (input == '\t') {
      insstr("        ");
    } else {
      insch(input);
    }
  } else if (vm->state == 2) {
    addch(input);
  }
  refresh();
}

void View::printTextChar(int input, int prevChar) {
  pair<int, int> loc = vm->updateLoc();
  if (input == KEY_BACKSPACE) {
    move(loc.first, loc.second);
    if (prevChar == '\t')
      addch('\t');
    else if (prevChar != 0)
      addch(' ');
  } else {
    addch(vm->text[vm->vcursor.getRow()][vm->vcursor.getCol() - 1]);
  }
  refresh();
}

}  // namespace CS246E