#include <scene/ui/EventListener.h>
#include <scene/ui/widget/Widget.h>

namespace urchin {

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

    }

}
