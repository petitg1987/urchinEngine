#include <3d/scene/ui/FocusEventListener.h>

FocusEventListener::FocusEventListener(bool& focused, bool& focusLost) :
        focused(focused),
        focusLost(focusLost) {

}

bool FocusEventListener::onFocus(urchin::Widget*) {
    focused = true;
    return true;
}

bool FocusEventListener::onFocusLost(urchin::Widget*) {
    focusLost = true;
    return true;
}
