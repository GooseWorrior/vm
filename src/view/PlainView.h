#ifndef PLAIN_VIEW_H
#define PLAIN_VIEW_H

#include "../editorComponent/VM.h"
#include "View.h"
namespace CS246E {
    class PlainView : public View {
        PlainView();
        void update() override;
        void display() override;
        ~PlainView();
    };
}  // namespace CS246E
#endif
