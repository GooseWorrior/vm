#ifndef VM_H
#define VM_H

#include <string>

#include "../Model.h"

#include "Commandline.h"
#include "Cursor.h"
#include "EditorComponent.h"
#include "StatusLine.h"

using std::string;

namespace CS246E {
class VM : public Model {
  string text;

 public:
  VM(string filename);
  void process();
};
}  // namespace CS246E
#endif
