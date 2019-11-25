#ifndef EDITOR_COMPONENT_H
#define EDITOR_COMPONENT_H

#include "StatusLine.h"
#include "Cursor.h"
#include <vector>
#include <initializer_list>
#include <memory>

using std::unique_ptr;
using std::initializer_list;
using std::vector;

namespace CS246E {
class EditorComponent {
    pair<int, int> & winSize;
    pair<int, int> & winPtr;
    Cursor & vcursor;
    vector<string> & theText;
    int & state;
    string VMCommandLine; // should be string &, change later 
    string VMStatusLine; //should be string &, change later
    vector<pair<int, unique_ptr<StatusLine>>> components;
    public:
      EditorComponent(pair<int, int> & winSize, pair<int, int> & winPtr, Cursor & vcursor, vector<string> & text, int & state,
      string & VMCommandLine, string & VMStatusLine);
      void reset();
      void addElement(initializer_list<int> types);
      void deleteElement(initializer_list<int> types);
      pair<int, int> getLocation(int type);
      string getContents(int type);
      bool updateContents();
      bool updateLocation();
      void update();
      void print();
};
}  // namespace CS246E
#endif
