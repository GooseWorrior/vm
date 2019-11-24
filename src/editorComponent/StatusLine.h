#ifndef STATUS_LINE_H
#define STATUS_LINE_H
#include <utility>
#include <string>

using std::pair;
using std::string;
namespace CS246E {
class StatusLine {
  pair<int, int> loc;
  string str;
  public: 
  StatusLine(string str, pair<int, int> loc);
  void print();
  void setContents(string ref, bool & flag);
  void setLocation(pair<int, int> ref, bool & flag);
};
}  // namespace CS246E
#endif
