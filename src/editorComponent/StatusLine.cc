#include "statusLine.h"
#include <ncurses.h>

using std::pair;
namespace CS246E {
    StatusLine::StatusLine(string str, pair<int, int> loc): str{str}, loc{loc} {}
    void StatusLine::print() {
        move(loc.first, loc.second);
        addstr(str.c_str());
    }
    void StatusLine::setContents(string ref, bool & flag) {
        flag = str != ref;
        str = ref;
    } 
    void StatusLine::setLocation(pair<int, int> ref, bool & flag) {
        flag = loc != ref;
        loc = ref;
    }
}  // namespace CS246E
