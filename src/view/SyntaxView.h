#ifndef SYNTAX_VIEW_H
#define SYNTAX_VIEW_H

#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "View.h"

using std::map;
using std::regex;
using std::regex_search;
using std::smatch;
using std::string;
using std::unique_ptr;
using std::vector;

namespace CS246E {
class VM;
class SyntaxView : public View {
  vector<pair<int, pair<regex, int>>> renderLibrary;
  void loadLibrary();  // move to View
  void render(
      vector<pair<int, int>>& singleComment,
      vector<pair<bool, pair<int, int>>>& multiComment);  // move to View
  pair<int, int> CloseBracket(char openBracket, char closeBracket,
                              pair<int, int> local);
  pair<int, int> OpenBracket(char openBracket, char closeBracket,
                             pair<int, int> local);

  void highlightBracket();
  bool inRange(int row);
  pair<int, int> transferLoc(int row, int col);

 public:
  SyntaxView(VM* vm);
  void update() override;
  ~SyntaxView();
  void initialize();
};
}  // namespace CS246E
#endif
