#ifndef VM_H
#define VM_H

#include <string>
#include <vector>

#include "../Model.h"

#include "Commandline.h"
#include "Cursor.h"
#include "EditorComponent.h"
#include "StatusLine.h"

using std::string;
using std::vector;

namespace CS246E {
class VM : public Model {
  vector<string> text;

 public:
  VM(string filename);
  void process();
};
}  // namespace CS246E
#endif
