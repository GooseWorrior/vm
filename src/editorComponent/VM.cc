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
VM::VM(string filename) : vcursor(0, 0, text) {
  // load files
  std::ifstream file{filename};
  file >> std::noskipws;

  char c;
  string line;

  while (file >> c) {
    if (c == '\n') {
      text.push_back(line);
      line.clear();
    } else {
      line += c;
    }
    addch(c);
  }
  text.push_back(line);  // pushes last line
  vcursor.setCursor(text.size() - 1, text.back().length());
  updateWindowSize();
}

void VM::process() {
  pair<int, int> prevCursor;
  int prevSize = 0;
  int input = 0;
  while (input != 'q') {
    input = controller->getChar();
    int prevChar = 0;
    bool edit = false;  // could be omitted
    prevSize = text.size();
    prevCursor = pair<int, int>(vcursor.getRow(), vcursor.getCol());
    switch (input) {
      case KEY_LEFT:
        --vcursor;
        break;
      case KEY_RIGHT:
        ++vcursor;
        break;
      case KEY_UP:
        vcursor.prevLine();
        break;
      case KEY_DOWN:
        vcursor.nextLine();
        break;
      case KEY_BACKSPACE:
        edit = true;
        prevChar = vcursor.erase();
        break;
      case 410:  // special resize character
        break;
      default:
        edit = true;
        vcursor.insert(input);
        break;
    }
    if (updateWindowSize()) {
      printTextAll();
    } else if (text.size() != prevSize) {
      printTextAfterward(input, prevCursor);
    } else if (edit && vcursor.getCol() != text[vcursor.getRow()].size()) {
      printTextLine(input, prevCursor, prevChar);
    } else if (edit) {
      printTextChar(input, prevChar);
    }

    pair<int, int> loc = updateLoc();
    move(loc.first, loc.second);
  }
}

bool VM::updateWindowSize() {
  int prevRow = WindowSize.first, prevCol = WindowSize.second;
  getmaxyx(stdscr, WindowSize.first, WindowSize.second);
  return prevRow != WindowSize.first || prevCol != WindowSize.second;
}

pair<int, int> VM::updateLoc() {
  int row = 0, col = 0, temp1 = 0;
  for (size_t i = 0; i <= vcursor.getRow(); ++i) {
    size_t temp2 = 0;
    if (i == vcursor.getRow()) {
      for (size_t j = 0; j < vcursor.getCol(); ++j) {
        temp2 += text[i][j] == '\t' ? 8 : 1;
        temp1 += text[i][j] == '\t' ? 8 : 1;
      }
    } else {
      for (size_t j = 0; j < text[i].size(); ++j) {
        temp2 += text[i][j] == '\t' ? 8 : 1;
      }
    }
    row += temp2 / WindowSize.second + 1;
  }
  row--;
  col = temp1 % WindowSize.second;
  return pair<int, int>(row, col);
}

void VM::printTextAll() {
  clear();
  for (auto i : text) {
    printw("%s\n", i.c_str());
  }
  refresh();
}

void VM::printTextAfterward(int input, pair<int, int> prevCursor) {
  clrtobot();
  refresh();
  for (size_t i = vcursor.getRow(); i < text.size(); ++i) {
    move(i, 0);
    clrtoeol();
    printw("%s\n", text[i].c_str());
    refresh();
  }
}

void VM::printTextLine(int input, pair<int, int> prevCursor, int prevChar) {
  if (input == KEY_BACKSPACE && prevChar == 0) return;
  pair<int, int> loc = updateLoc();
  if (input == KEY_BACKSPACE) {
    clrtoeol();
    move(loc.first, loc.second);
    for (size_t i = vcursor.getCol(); i < text[vcursor.getRow()].size(); ++i) {
      addch(text[vcursor.getRow()][i]);
    }
  } else {
    insch(input);
  }
  refresh();
}

void VM::printTextChar(int input, int prevChar) {
  pair<int, int> loc = updateLoc();
  if (input == KEY_BACKSPACE) {
    move(loc.first, loc.second);
    if (prevChar == '\t')
      addch('\t');
    else if (prevChar != 0)
      addch(' ');
  } else {
    addch(text[vcursor.getRow()][vcursor.getCol() - 1]);
  }
  refresh();
}
}  // namespace CS246E
