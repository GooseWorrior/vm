#ifndef CRUSH_H
#define CRUSH_H

#include "EditorComponent.h"
#include <vector>
#include <string>

using std::pair;
using std::vector;
using std::string;
namespace CS246E {
    class Cursor {
        pair<int, int> theCursor;
        vector<string> & theText;
        public:
            Cursor(int row, int col, vector<string> & theText);
            Cursor & operator++();
            Cursor & operator--();
            Cursor & nextLine();
            Cursor & prevLine();
            Cursor & insert(wchar_t c);
            int erase();
            int getRow();
            int getCol();
    };
}  // namespace CS246E
#endif
