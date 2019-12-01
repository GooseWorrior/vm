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

#include "Commandline.h"
//#include "Cursor.h"
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
  int state;  // 0 - command/readonly, 1 - insert, 2 - commandline
  int commandCursor;
  int searchPointer;
  int savedSize;
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
  EditorComponent theComponents;
  pair<int, int> WindowSize;
  pair<int, int> WindowPointer;
  vector<string> text;
  vector<unique_ptr<EditorComponent>> components;
  vector<FILE*> undoStack;                           // row text and which row
  vector<pair<pair<int, int>, time_t>> cursorStack;  // cursor position and time
  vector<pair<int, int>> searchLibrary;
  pair<vector<string>, bool> clipBoard;
  std::map<string, vector<int>> macroLibrary;
  pair<string, vector<int>> curMacro;
  std::stack<pair<string, vector<int>>> curPlay;
  std::stack<int> macroPointer;
  pair<int, int> undoCount;
  string vmStatusString;

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
  // void handleBufferCommands(int input);
  void handleBCTemplate(int input, int state);
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
  void loadCursor();
  void exeBufferCommand();
  void saveSearch();
  void searchMinusOne();
  void searchPlusOne();
  void findNear();
  void setFilenameStatus();

 public:
  VM(string filename);
  void process();
  bool updateWindowSize();
  pair<int, int> updateLoc();
  friend class SyntaxView;
  friend class PlainView;
};
}  // namespace CS246E
#endif
