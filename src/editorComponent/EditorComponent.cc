
#include "EditorComponent.h"
#include <ncurses.h>
#include <algorithm>
#include <initializer_list>

using std::find_if;
using std::initializer_list;
using std::make_unique;
using std::string;
using std::to_string;
using std::unique_ptr;

namespace CS246E {
const int WARNING = 0;
const int PERCENTAGE = 1;
const int MODE = 2;
const int LINENUMBER = 3;
const int COMMANDLINE = 4;
const int STATUS = 5;
EditorComponent::EditorComponent(pair<int, int> &winSize,
                                 pair<int, int> &winPtr, Cursor &vcursor,
                                 vector<string> &theText, int &state,
                                 string &VMStatusLine, string &VMCommandLine)
    : winSize{winSize},
      winPtr{winPtr},
      vcursor{vcursor},
      theText{theText},
      state{state},
      VMCommandLine{VMCommandLine},
      VMStatusLine{VMStatusLine} {}
void EditorComponent::reset() { components.clear(); }
void EditorComponent::deleteElement(initializer_list<int> types) {
  for (auto i : types) {
    auto iter = find_if(components.begin(), components.end(),
                        [&i](pair<int, unique_ptr<StatusLine>> &ref) {
                          return ref.first == i;
                        });
    components.erase(iter);
  }
}
void EditorComponent::addElement(initializer_list<int> types) {
  for (auto i : types) {
    components.push_back(pair<int, unique_ptr<StatusLine>>(
        i, std::move(make_unique<StatusLine>(getContents(i), getLocation(i)))));
  }
}
pair<int, int> EditorComponent::getLocation(int type) {
  int row = winSize.first,
      col = 0;  // little trick here: due to updateWindowSize in VM
                // winSize.first == trueSize - 1
  switch (type) {
    case PERCENTAGE:
      col = winSize.second - 4;
      break;
    case MODE:
      col = 0;
      break;
    case LINENUMBER:
      col = winSize.second - 18;
      break;
    case STATUS:
      col = 0;
      break;
    case COMMANDLINE:
      col = 0;
      break;
    default:
      break;
  }
  return pair<int, int>(row, col);
}
string EditorComponent::getContents(int type) {
  string ret;
  switch (type) {
    case PERCENTAGE:
      if (winPtr.second == theText.size() - 1 && winPtr.first == 0) {
        ret = "All";
      } else if (winPtr.second == theText.size() - 1) {
        ret = "Bot";
      } else if (winPtr.first == 0) {
        ret = "Top";
      } else if (theText.size() == 1) {
        ret = "";
      } else {
        ret =
            to_string((int)((100 * (vcursor.getRow() + 1)) / theText.size())) +
            "%";
      }
      break;
    case MODE:
      ret = (state == 1) ? "-- INSERT --" : (state == 2) ? "-- REPLACE --" : "";
      break;
    case LINENUMBER:
      ret = to_string(vcursor.getRow() + 1) + "," +
            to_string(vcursor.getCol() + 1);
      break;
    case STATUS:
      ret = VMStatusLine;
    case COMMANDLINE:
      ret = VMCommandLine;
    default:
      break;
  }
  return ret;
}
bool EditorComponent::updateContents() {
  bool flag = false;
  for (auto &i : components) {
    i.second->setContents(getContents(i.first), flag);
  }
  return flag;
};
bool EditorComponent::updateLocation() {
  bool flag = false;
  for (auto &i : components) {
    i.second->setLocation(getLocation(i.first), flag);
  }
  return flag;
};
void EditorComponent::update() {
  bool flag1 = false, flag2 = false;
  mvaddstr(winSize.first - 1, 0, string(winSize.second, ' ').c_str());
  for (auto &i : components) {
    i.second->setLocation(getLocation(i.first), flag1);
    i.second->setContents(getContents(i.first), flag2);
    if (flag1 || flag2) {
      if (i.first == MODE) {
        attron(A_BOLD);
        i.second->print();
        attroff(A_BOLD);
      }
      i.second->print();
    }
  }
  refresh();
};
void EditorComponent::print() {
  mvaddstr(winSize.first, 0,
           string(winSize.second, ' ').c_str());  // little trick here
  for (auto &i : components) {
    if (i.first == MODE) {
      attron(A_BOLD);
      i.second->print();
      attroff(A_BOLD);
    }
    i.second->print();
  }
  refresh();
};
}  // namespace CS246E
