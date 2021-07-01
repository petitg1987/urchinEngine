#include <widget/controller/mouse/MouseController.h>
#include <scene/SceneDisplayerWindow.h>

namespace urchin {

    MouseController::MouseController(QWindow* widget) :
            widget(widget) {

    }

    void MouseController::moveMouse(int x, int y) {
        QCursor::setPos(widget->mapToGlobal(QPoint(x, y)));
    }

    Point2<int> MouseController::getMousePosition() const {
        QPoint mousePosition = widget->mapFromGlobal(QCursor::pos());
        return Point2<int>(mousePosition.x(), mousePosition.y());
    }
}
