#include "3d/scene/ui/VirtualKeyboardFake.h"
using namespace urchin;

VirtualKeyboardFake::VirtualKeyboardFake(bool& keyboardVisible) :
        keyboardVisible(keyboardVisible) {

}

void VirtualKeyboardFake::show(VirtualKeyboardType, const std::optional<Rectangle2D<int>>&) {
    keyboardVisible = true;
}

void VirtualKeyboardFake::hide() {
    keyboardVisible = false;
}
