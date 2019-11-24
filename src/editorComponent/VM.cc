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
VM::VM(string filename) : state{0}, vcursor(0, 0, text, WindowPointer, WindowSize),
 theComponents{WindowSize, WindowPointer, vcursor, text, state} {
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
  }
  text.push_back(line);  // pushes last line
  WindowPointer = pair<int, int>(0, text.size() - 1);

  printTextAll();

  vcursor.setCursor(text.size() - 1, text.back().length());
  updateWindowSize();

  printPlaceholder();
  
  theComponents.addelement({3, 1});
  theComponents.print();
  
  pair<int, int> loc = updateLoc();
  move(loc.first, loc.second);
  vcursor.updatePointer(1);
}

void VM::process() {
  pair<int, int> prevWindowSize;
  pair<int, int> prevCursor;
  pair<int, int> prevPointer;
  int prevState = 0;
  int prevSize = 0;
  int input = 0;
  while (input != 'q') {
    input = controller->getChar();
    int prevChar = 0;
    bool edit = false;  // could be omitted
    prevSize = text.size();
    prevCursor = pair<int, int>(vcursor.getRow(), vcursor.getCol());
    prevPointer = WindowPointer;
    prevWindowSize = WindowSize;
    prevState = state;
    switch (input) {
      case 'a':
        state = 1;
        break;
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
      case 36:  // dollar $
        if (state == 0) {
          // set to end of line
          vcursor.setCursor(vcursor.getRow(),
                            text[vcursor.getRow()].length() - 1);
        }
        break;
      case 48:
        if (state == 0) {
          // set to start of line
          vcursor.setCursor(vcursor.getRow(), 0);
        }
        break;
      case 37:  // percentage %
        if (state == 0) {
          vcursor.handlePercentage(text[vcursor.getRow()][vcursor.getCol()]);
        }
        break;
      case 94:  // caret ^
        if (state == 0) {
          vcursor.handleCaret();
        }
        break;
      case 70:  // big F
        if (state == 0) {
          vcursor.handleF(getch());
        }
        break;
      case 102:  // little f
        if (state == 0) {
          vcursor.handlef(getch());
        }
        break;
      case 59:  // semi colon ;
        if (state == 0) {
          vcursor.handleSemiColon();
        }
        break;
      case 27:  // escape
        state = 0;
        break;
      case 58:  // colon
        if (state == 0) {
          state = 3;
        }
      default:
        if (state == 1) {
          edit = true;
          vcursor.insert(input);
        }
        break;
    }
    //WindowPointer.second - WindowPointer.first + 1 < text.size() ??? code
    updateWindowSize();
    vcursor.updatePointer(1);
    if (!edit) vcursor.updatePointer(0);
    if (prevWindowSize != WindowSize ||
        prevPointer.first < WindowPointer.first ||
        prevPointer.second > WindowPointer.second) {
      printTextAll();
    } else if (text.size() != prevSize) {
      printTextAfterward(input, prevCursor);
    } else if (edit && vcursor.getCol() != text[vcursor.getRow()].size()) {
      printTextLine(input, prevCursor, prevChar);
    } else if (edit) {
      printTextChar(input, prevChar);
    }
    
    printPlaceholder();

    if (prevState != state) {
      if (state == 1) {
        theComponents.reset();   
        theComponents.addelement({2, 3, 1});
      } else if (state == 0) {
        theComponents.reset();   
        theComponents.addelement({3, 1});
      }
    } 
    theComponents.updateContents();
    theComponents.updateLocation();
    theComponents.print();
    //theComponents.update();
    std::ofstream f;
    f.open("debug.txt");
    for (auto &i : text) {
      f << i << "\n";
    }
    f << vcursor.getRow() << " " << vcursor.getCol() << " "
      << WindowPointer.first << " " << WindowPointer.second << " "
      << text.size() << " " << WindowSize.first << "\n";
    pair<int, int> loc = updateLoc();
    move(loc.first, loc.second);
  }
}

bool VM::updateWindowSize() {
  int prevRow = WindowSize.first, prevCol = WindowSize.second;
  getmaxyx(stdscr, WindowSize.first, WindowSize.second);
   WindowSize.first--; // little trick here
  return prevRow != WindowSize.first || prevCol != WindowSize.second;
}

void VM::printPlaceholder() {
    attron(COLOR_PAIR(1));
    for (int i = vcursor.calculateLine(); i < WindowSize.first; i++) {
      mvaddch(i, 0,'~');
    }
    refresh();
    attroff(COLOR_PAIR(1));
}
pair<int, int> VM::updateLoc() {
  int row = 0, col = 0, temp1 = 0;
  for (size_t i = WindowPointer.first; i <= vcursor.getRow(); ++i) {
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
  /*for (auto i : text) {
    printw("%s\n", i.c_str());
  }*/
  for (size_t i = WindowPointer.first; i <= WindowPointer.second; ++i) {
    printw("%s\n", text[i].c_str());
  }
  refresh();
}

void VM::printTextAfterward(int input, pair<int, int> prevCursor) {
  std::ofstream f;
  f.open("debug.txt");
  for (auto &i : text) {
    f << i << "\n";
  }
  f << vcursor.getRow() << " " << vcursor.getCol() << WindowPointer.first << " "
    << WindowPointer.second << "\n";
  clrtobot();
  refresh();
  pair<int, int> loc = updateLoc();
  move(loc.first, 0);
  for (size_t i = vcursor.getRow(); i <= WindowPointer.second; ++i) {
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
  std::ofstream f;
  f.open("debug.txt");
  for (auto &i : text) {
    f << i << "\n";
  }
  f << vcursor.getRow() << " " << vcursor.getCol() << "\n";
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
