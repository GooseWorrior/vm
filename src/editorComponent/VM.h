#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <time.h>
#include <map>
#include <string>
#include <vector>

#include "../Model.h"
#include "../view/PlainView.h"
#include "../view/SyntaxView.h"

#include <regex>
#include "EditorComponent.h"
#include "StatusLine.h"

using std::pair;
using std::regex;
using std::string;
using std::unique_ptr;
using std::vector;

namespace CS246E {
class VM : public Model {
  class Undo {
   public:
    FILE* undoData;
    pair<int, int> cursorPos;
    time_t timer;
    Undo(FILE* undoData, pair<int, int> cursorPos, time_t timer)
        : undoData{undoData}, cursorPos{cursorPos}, timer{timer} {};
  };
  int state;  // 0 - command/readonly, 1 - insert
  int commandCursor;
  int searchPointer;
  int savedSize;
  pair<int, int> lastCommand;  // <command char, input char (if necessary)>
  string lastBufferCommand;
  bool exitCode;
  bool CFile;
  bool searchDirection;  // 0 backwards, 1 forwards
  bool recordOn;
  bool playOn;
  string fileName;
  string bufferCommand;
  string errorMessage;
  string pattern;
  Cursor vcursor;
  pair<string, vector<int>> curMacro;
  EditorComponent theComponents;
  pair<int, int> WindowSize;
  pair<int, int> WindowPointer;
  vector<string> text;
  vector<unique_ptr<EditorComponent>> components;
  vector<unique_ptr<Undo>> undoStack;  // cursor position and time
  vector<pair<int, int>> searchLibrary;
  pair<vector<string>, bool> clipBoard;
  std::map<string, vector<int>> macroLibrary;
  std::stack<pair<string, vector<int>>> curPlay;
  std::stack<int> macroPointer;
  std::stack<pair<int, int>> macroLocation;
  pair<int, int> undoCount;
  string vmStatusString;

  bool pureMacro(string type);
  void forcePrint();
  void exeMotionDelete(pair<int, int> ref);
  void exeMotionCopy(pair<int, int> ref);
  void handleMultiplier(string cmd);
  void handleMotionDelete(bool mode, string cmd);
  void handleMotionCopy(string cmd);
  void changeState(int mode);
  int macroGets();
  void checkPlayEnd();
  int checkLineLength(int x, int lineLength);
  bool checkExists(string file);
  bool isNumber(const string& str);
  bool isCFile();
  void writeFile(string file);
  void copyFile(string file);
  void findPairedBracket();
  void handleCommands(int input, bool* shouldSave);
  void handleBCTemplate(int input, int state);
  void handleNoEditBC(int input);
  void handleGeneralBC();
  void handleSearchForward();
  void handleSearchBackward();
  void handleRecordMacro();
  void handleRecordEnd();
  void handlePlayMacro();
  void loadMacro(int input);
  void loadFile(string filename);
  void saveText();
  void saveRow();
  void loadUndo();
  void exeBufferCommand();
  void saveSearch();
  void searchMinusOne();
  void searchPlusOne();
  void findNear();
  void setFilenameStatus();
  bool updateWindowSize();

 public:
  VM(string filename);
  void process();
  pair<int, int> updateLoc();
  friend class SyntaxView;
  friend class PlainView;
};
}  // namespace CS246E
#endif
