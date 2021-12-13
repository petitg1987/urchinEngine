#include <3d/scene/ui/FocusEventListener.h>
using namespace urchin;

FocusEventListener::FocusEventListener(bool& focused, bool& focusLost) :
        focused(focused),
        focusLost(focusLost) {

}

bool FocusEventListener::onFocus(Widget*) {
    focused = true;
    return true;
}

bool FocusEventListener::onFocusLost(Widget*) {
    focusLost = true;
    return true;
}
