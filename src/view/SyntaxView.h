#ifndef SYNTAX_VIEW_H
#define SYNTAX_VIEW_H

#include <memory>
#include <vector>
#include <string>
#include <map> 
#include <regex>

#include "../editorComponent/VM.h"
#include "View.h"

using std::map;
using std::string;
using std::vector;
using std::unique_ptr;
using std::regex;
using std::smatch;
using std::regex_search;

namespace CS246E {
class VM;
class SyntaxView : public View {
  VM * vm;
  vector<pair<int, pair<regex, int>>> renderLibrary;
 public:
  SyntaxView(VM * vm);
  void update() override;
  void display(pair<int, int> prevPointer, int input, pair<int, int> prevCursor,
               pair<int, int> prevWindowSize, int prevChar, int prevSize, bool edit) override;
  ~SyntaxView();
  
  // transport functions
  void printTextAll();  // temporary
  void printTextAfterward(int input, pair<int, int> prevCursor);
  void printTextLine(int input, pair<int, int> prevCursor,
                     int prevChar);  // temporary
  void printTextChar(int input, int prevChar);
  void printPlaceholder(); 

  void initialize();
  void loadLibrary(); // move to View
  void render(vector<pair<int, int>> & singleComment,  vector<pair<bool, pair<int, int>>> & multiComment); // move to View
  pair<int, int> CloseBracket(char openBracket, char closeBracket, pair<int, int> local);
  pair<int, int> OpenBracket(char openBracket, char closeBracket, pair<int, int> local);
  void highlightBracket();
  pair<int, int> transferLoc(int row, int col);

};
}  // namespace CS246E
#endif
