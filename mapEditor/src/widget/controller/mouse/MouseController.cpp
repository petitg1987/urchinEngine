#include "MouseController.h"
#include "scene/SceneDisplayerWidget.h"

namespace urchin {
    MouseController::MouseController(QWidget* widget) :
            widget(widget) {

    }

    void MouseController::moveMouse(unsigned int x, unsigned int y) {
        QCursor::setPos(widget->mapToGlobal(QPoint((int)x, (int)y)));
    }

    Point2<unsigned int> MouseController::getMousePosition() const {
        QPoint mousePosition = widget->mapFromGlobal(QCursor::pos());
        return Point2<unsigned int>((unsigned int)mousePosition.x(), (unsigned int)mousePosition.y());
    }
}
