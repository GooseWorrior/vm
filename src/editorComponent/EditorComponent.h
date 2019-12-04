#ifndef EDITOR_COMPONENT_H
#define EDITOR_COMPONENT_H

#include <initializer_list>
#include <memory>
#include <vector>
#include "Cursor.h"
#include "StatusLine.h"

using std::initializer_list;
using std::unique_ptr;
using std::vector;

namespace CS246E {
class EditorComponent {
  pair<int, int>& winSize;
  pair<int, int>& winPtr;
  Cursor& vcursor;
  vector<string>& theText;
  int& state;
  string& VMCommandLine;  // should be string &, change later
  string& VMStatusLine;   // should be string &, change later
  string& VMErrorMessage;
  string& VMMacroName;
  vector<pair<int, unique_ptr<StatusLine>>> components;
  void update();
  string getContents(int type);
  void reset();

 public:
  EditorComponent(pair<int, int>& winSize, pair<int, int>& winPtr,
                  Cursor& vcursor, vector<string>& text, int& state,
                  string& VMStatusLine, string& VMCommandLine,
                  string& errorMessage, string& VMMacroName);
  void addElement(initializer_list<int> types);
  void removeElement(initializer_list<int> types);
  pair<int, int> getLocation(int type);
  bool updateContents();
  bool updateLocation();
  void print();
};
}  // namespace CS246E
#endif
