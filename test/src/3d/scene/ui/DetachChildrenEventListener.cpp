#include "3d/scene/ui/DetachChildrenEventListener.h"
using namespace urchin;

DetachChildrenEventListener::DetachChildrenEventListener(Widget* widget) :
        widget(widget) {

}

bool DetachChildrenEventListener::onMouseLeftClick(Widget*) {
    widget->detachChildren();
    return false;
}
