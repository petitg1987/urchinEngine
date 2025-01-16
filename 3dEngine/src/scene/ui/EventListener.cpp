#include <scene/ui/EventListener.h>

namespace urchin {

    void EventListener::onInit(Widget*) {

    }

    bool EventListener::onMouseLeftClick(Widget*) {
        return true;
    }

    bool EventListener::onMouseLeftClickRelease(Widget*) {
        return true;
    }

    bool EventListener::onFocus(Widget*) {
        return true;
    }

    bool EventListener::onFocusLost(Widget*) {
        return true;
    }

    void EventListener::onValueChange(Widget*) {
        //can be overridden
    }

}
