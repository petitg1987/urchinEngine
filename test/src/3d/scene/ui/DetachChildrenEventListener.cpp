#include <3d/scene/ui/DetachChildrenEventListener.h>
using namespace urchin;

DetachChildrenEventListener::DetachChildrenEventListener(Widget* widget) :
        widget(widget) {

}

void DetachChildrenEventListener::onMouseLeftClick(Widget*) {
    widget->detachChildren();
}
