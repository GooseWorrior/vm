#ifndef STATUS_LINE_H
#define STATUS_LINE_H

#include "EditorComponent.h"

namespace CS246E {
class StatusLine : public EditorComponent {
  pair<int, int> cursorLoc;
};
}  // namespace CS246E
#endif
