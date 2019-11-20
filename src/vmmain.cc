#include <iostream>
#include <memory>
#include <string>

#include "EditorComponent/VM.h"

using std::string;
using std::unique_ptr;

int main(int argc, char *argv[]) {
  // might change later if doing multiple files
  if (argc != 2) {
    return 1;
  }
  string filename = argv[1];
  unique_ptr<CS246E::VM> model = std::make_unique<CS246E::VM>(filename);
}
