#ifndef SYNTAX_VIEW_H
#define SYNTAX_VIEW_H

#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "../editorComponent/VM.h"
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
  VM* vm;
  vector<pair<int, pair<regex, int>>> renderLibrary;
  void printTextAfterward(int input, pair<int, int> prevCursor);
  void printTextLine(int input, pair<int, int> prevCursor,
                     int prevChar);  // temporary
  void printTextChar(int input, int prevChar);
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
  void display(pair<int, int> prevPointer, int input, pair<int, int> prevCursor,
               pair<int, int> prevWindowSize, int prevChar, int prevSize,
               bool edit) override;
  ~SyntaxView();

  // transport functions
  void printTextAll();  // temporary
  void printPlaceholder();
  void initialize();
};
}  // namespace CS246E
#endif
