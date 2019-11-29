#include <ncurses.h>
#include <fstream>
#include <iostream>  // remove after debugging

#include <sstream>
#include "../controller/Keyboard.h"
#include "VM.h"
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <regex>
#include <memory>

using std::make_unique;
using std::find_if;
using std::atoi;
using std::isdigit;
using std::stringstream;
using std::to_string;
using std::smatch;
using std::regex;
using std::regex_search;

namespace CS246E {
VM::VM(string filename)
    : state{0},
      commandCursor{0},
      savedSize{0},
      fileName{filename},
      exitCode{1},
      CFile{0},
      vcursor(0, 0, text, WindowPointer, WindowSize, state),
      theComponents{WindowSize, WindowPointer,  vcursor,      text,
                    state,      vmStatusString, bufferCommand, errorMessage} {
  CFile = isCFile();
    if (!CFile) {
      addView(make_unique<PlainView>(this));
    } else {
    addView(make_unique<SyntaxView>(this));
    view->initialize();
  }
  loadFile(filename);
}

int ifNegativeThenZero(int x);

void VM::loadFile(string filename) {
  // load files
  fileName = filename; // in case we change file
  CFile = isCFile();
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
  //WindowPointer = pair<int, int>(0, text.size() - 1);
  
  //clear();
  //refresh();
  //view->printTextAll();
  
  // std::ofstream f1;
  // f1.open("debug.txt");
  // for (auto i : text) {
  //   f1 << i << '\n';
  // }
  // f1.close();
  if (checkExists(filename)) {
    int count = 0;
    for (auto i : text) count += i.size();
    vmStatusString = "\"" + filename + "\" " + to_string(text.size()) + "L, " + to_string(count) + "C";
  } else {
    vmStatusString = "\"" + filename + "\" " + "[New File]";
  }
  
  if (CFile) vmStatusString += " [CFile]";

  vcursor.setCursor(text.size() - 1, ifNegativeThenZero(text.back().length() - 1));
  updateWindowSize();
  
  WindowPointer = pair<int, int>(0, std::min<int>(text.size(), WindowSize.first) - 1);

  vcursor.updatePointer(0);
  clear();
  refresh();
  view->printTextAll();
  view->printPlaceholder();
  view->update();

  theComponents.addElement({5, 3, 1});
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
  bool shouldSave = true;

  while (exitCode && input != '|') {
    input = controller->getChar();
    int prevChar = 0;
    bool edit = false;  // could be omitted
    if (state == 1 || state == 2 || state == 0)
      prevCursor = pair<int, int>(vcursor.getRow(), vcursor.getCol());
    // only keep track of the last cursor location in insert or replace mode
    prevSize = text.size();
    prevPointer = WindowPointer;
    prevWindowSize = WindowSize;
    prevState = state;
    if (state == 3) {
      handleBufferCommands(input);
    } else
      switch (input) {
        case KEY_LEFT:
          if (vcursor.getCol() != (--vcursor).getCol()) {
            shouldSave = true;
          }
          break;
        case KEY_RIGHT:
          if (vcursor.getCol() != (++vcursor).getCol()) {
            shouldSave = true;
          }
          break;
        case KEY_UP:
          if (vcursor.getRow() != vcursor.prevLine().getRow()) {
            shouldSave = true;
          }
          break;
        case KEY_DOWN:
          if (vcursor.getRow() != vcursor.nextLine().getRow()) {
            shouldSave = true;
          }
          break;
        case KEY_BACKSPACE:
          edit = true;
          prevChar = vcursor.erase();
          break;
        case 410:  // special resize character
          break;
        case 27:  // escape
          if (state == 1 && vcursor.getCol() > 0) {
            vcursor.setCursor(vcursor.getRow(), vcursor.getCol() - 1);
          }
          state = 0;
          vcursor.updateStateOffset(-1);
          break;
        default:
          if (state == 0) {
            handleCommands(input);
          } else if (state == 1 || state == 2) {
            edit = true;
            if (shouldSave) {
              saveText();
              shouldSave = false;
            }
            vcursor.insert(input);
          }
      }
    // WindowPointer.second - WindowPointer.first + 1 < text.size() ??? code
    updateWindowSize();
    vcursor.updatePointer(-1);
    vcursor.updatePointer(1);
    if (!edit) vcursor.updatePointer(0);
    view->display(prevPointer, input, prevCursor, prevWindowSize, prevChar, prevSize, edit);

    if (prevState != state) {
      if (prevState == 1 || prevState == 2) {
        theComponents.deleteElement({2, 3, 1});
      } else if (prevState == 3) {
        theComponents.deleteElement({4});
      } else if (prevState == 0) {
        theComponents.deleteElement({0, 5, 3, 1});
        vmStatusString.clear();
        errorMessage.clear();
      }
      if (state == 1 || state == 2) {
        // theComponents.reset();
        // theComponents.addelement({2, 3, 1});
        theComponents.addElement({2, 3, 1});
      } else if (state == 3) {
        // theComponents.reset();
        // theComponents.addelement({3, 1});
        theComponents.addElement({4});
      } else if (state == 0) {
        theComponents.addElement({5, 3, 1});
      }
    }
    
    if (!vmStatusString.empty()) errorMessage.clear(); // errorMessage guard
    theComponents.updateContents();
    theComponents.updateLocation();
    theComponents.print();
    // theComponents.deleteElement({0});
    // theComponents.update();

    //render();

    if (state == 3) {
      move(WindowSize.first, commandCursor);
    } else {
      pair<int, int> loc = updateLoc();
      move(loc.first, loc.second);
    }
  }
}



bool VM::checkExists(string file) {
    std::ifstream file_to_check(file.c_str());
    if(file_to_check.is_open()) {
      file_to_check.close();
      return true;
    }
    file_to_check.close();
    return false;
}

void VM::writeFile(string file) {
  std::fstream output;
  output.open(file, std::fstream::out);
  for (int i = 0; i < text.size(); ++i) {
    output << text[i];
    if (i != text.size() - 1) output << '\n';
  }
  output.close();
}

void VM::copyFile(string filename) {
  int Row = vcursor.getRow() + 1;
  int curRow = Row;
  std::ifstream file{filename};
  file >> std::noskipws;
  char c;
  string line;
  while (file >> c) {
    if (c == '\n') {
      text.insert(text.begin() + Row, line);
      line.clear();
      curRow++;
    } else {
      line += c;
    }
  }
  text.insert(text.begin() + curRow, line);
  vcursor.setCursor(Row, 0);
}
 
bool VM::isNumber(const string& s) {
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

bool VM::isCFile() {
    smatch m;
    regex reg[3];
    reg[0] = regex(".cc$");
    reg[1] = regex(".c$");
    reg[2] = regex(".h$");
    for (int i = 0; i < 3; i++) {
        regex_search (fileName, m, reg[i]);
        if (m.size() > 0) return true; 
    }
    return false;
}
void VM::exeBufferCommand() {
  while (bufferCommand[0] == ':') bufferCommand = bufferCommand.substr(1); 
  stringstream source{bufferCommand};
  string cmd;
  if (source >> cmd) {
    if (cmd == "w") {
       savedSize = undoStack.size();
       writeFile(fileName);
       int count = 0;
       for (auto i : text) count += i.size();
       vmStatusString = "\"" + fileName + "\" " + to_string(text.size()) + "L, " + to_string(count) + "C" + " written";
    } else if (cmd == "q") {
       if (savedSize != undoStack.size()) {
           //std::fstream f{"debug.txt"};
           //f << savedSize << " " << undoStack.size();
           theComponents.addElement({0});
           errorMessage = "E37: No write since last change (add ! to override)";
       } else {
           exitCode = 0;
       }
    } else if (cmd == "wq") {
      writeFile(fileName);
      exitCode = 0;
    } else if (cmd == "q!") {
      exitCode = 0;
    } else if (cmd == "$") {
      vcursor.setCursor(text.size() - 1, 0); 
    } else if (cmd == "r") {
      source >> cmd;
      if (checkExists(cmd)) {
        copyFile(cmd);
        saveText();
      } else {
        theComponents.addElement({0});
        errorMessage = "E484: Can't open file " + cmd;
      }
    } else if (isNumber(cmd)) {
      int row = atoi(cmd.c_str());
      row = (row < 1) ? 1 : row;
      row = (row > text.size()) ? text.size() : row;
      vcursor.setCursor(row - 1, 0);
    } else {
      theComponents.addElement({0});
      errorMessage = "E492: Not an editor command: " + cmd;
    }
  }
  state = 0;
}

void VM::handleBufferCommands(int input) {
  switch (input) {
    case KEY_LEFT:
      if (commandCursor > 1) commandCursor--;
      break;
    case KEY_RIGHT:
      if (commandCursor < bufferCommand.size()) commandCursor++;
      break;
    case KEY_BACKSPACE:
      if (bufferCommand.size() == 1) {
        state = 0;
      } else if (commandCursor > 1) {
        bufferCommand.erase(commandCursor - 1, 1);
        commandCursor--;
      }
      break;
    case '\n':
      exeBufferCommand();
      bufferCommand.clear();
      state = 0;
      commandCursor = 0;
      break;
    default:
      bufferCommand.insert(commandCursor, 1, input);
      commandCursor++;
  }
}

void VM::handleCommands(int input) {
  switch (input) {
    case 65:  // A
      vcursor.setCursor(vcursor.getRow(), text[vcursor.getRow()].length());
      state = 1;
      vcursor.updateStateOffset(0);
      break;
    case 73:  // I
      vcursor.setCursor(vcursor.getRow(), 0);
      state = 1;
      vcursor.updateStateOffset(0);
      break;
    case 97:  // a
      vcursor.setCursor(vcursor.getRow(),
                        vcursor.getCol() ? vcursor.getCol() + 1 : 0);
      state = 1;
      vcursor.updateStateOffset(0);
      break;
    case 98:  // b
      vcursor.handleb();
      break;
    case 105:  // i
      state = 1;
      vcursor.updateStateOffset(0);
    case 36:  // dollar $
      // set to end of line
      vcursor.setCursor(
          vcursor.getRow(),
          ifNegativeThenZero(text[vcursor.getRow()].length() - 1));
      break;
    case 48:
      // set to start of line
      vcursor.setCursor(vcursor.getRow(), 0);
      break;
    case 37:  // percentage %
      vcursor.handlePercentage(text[vcursor.getRow()][vcursor.getCol()]);
      break;
    case 94:  // caret ^
      vcursor.handleCaret();
      break;
    case 70:  // big F
      vcursor.handleF(getch());
      break;
    case 102:  // little f
      vcursor.handlef(getch());
      break;
    case 59:  // semi colon ;
      vcursor.handleSemiColon();
      break;
    case 58:      // colon
      state = 3;  // commandline state
      commandCursor = 1;
      bufferCommand = ":";
      break;
    case 82:  // R
      state = 2;
      vcursor.updateStateOffset(0);
      break;
    case 117:
      loadUndo();
      loadCursor();
      break;
    case 104:  // h
      --vcursor;
      break;
    case 106:  // j
      vcursor.nextLine();
      break;
    case 107:  // k
      vcursor.prevLine();
      break;
    case 108:  // l
      ++vcursor;
      break;
  }
}

void VM::saveText() {
  string command = "mktemp";
  FILE *f = popen(command.c_str(), "r");
  string tempDir;
  for (char c = fgetc(f); c != EOF; c = fgetc(f)) {
    tempDir += c;
  }
  std::ofstream tempFile;
  tempFile.open(tempDir);
  for (int i = 0; i < text.size(); ++i) {
    if (i != text.size() - 1) {
      tempFile << text[i] << "\n";
    } else {
      tempFile << text[i];
    }
  }
  tempFile.close();
  undoStack.push_back(tempDir);

  time_t timer;
  time(&timer);
  cursorStack.push_back(std::make_pair(
      std::make_pair(vcursor.getRow(), vcursor.getCol()), timer));
}

void VM::loadUndo() {
  if (undoStack.size()) {
    text.clear();
    loadFile(undoStack.back());
    if (undoStack.size() >= undoCount.second) {
      undoCount = std::make_pair(undoCount.first, undoStack.size());
    }
    vmStatusString = "1 change; before #" +
                     std::to_string(undoCount.first + undoStack.size()) + "  ";
    undoStack.pop_back();
    if (!undoStack.size()) {
      undoCount = std::make_pair(undoCount.first + undoCount.second, 0);
    }
  } else {
    vmStatusString = "Already at oldest change";
  }
}

void VM::loadCursor() {
  if (cursorStack.size()) {
    int row = cursorStack.back().first.first;
    int col = cursorStack.back().first.second;
    vcursor.setCursor(
        row, text[row].length() == col && text[row] != "" ? col - 1 : col);
    time_t currentTime;
    time(&currentTime);

    vmStatusString +=
        std::to_string((int)difftime(currentTime, cursorStack.back().second)) +
        " seconds ago";
    std::ofstream f;
    f.open("debug.txt");
    f << vmStatusString;
    cursorStack.pop_back();
  }
}

bool VM::updateWindowSize() {
  int prevRow = WindowSize.first, prevCol = WindowSize.second;
  getmaxyx(stdscr, WindowSize.first, WindowSize.second);
  WindowSize.first--;  // little trick here
  return prevRow != WindowSize.first || prevCol != WindowSize.second;
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

}  // namespace CS246E
