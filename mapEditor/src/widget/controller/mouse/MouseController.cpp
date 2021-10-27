#include <widget/controller/mouse/MouseController.h>
#include <scene/SceneDisplayerWindow.h>

namespace urchin {

    MouseController::MouseController(QWindow* widget) :
            widget(widget) {

    }

    void MouseController::moveMouse(int x, int y) {
        int mouseXDpiAdjusted = MathFunction::roundToInt((float)x / (float)widget->devicePixelRatio());
        int mouseYDpiAdjusted = MathFunction::roundToInt((float)y / (float)widget->devicePixelRatio());
        QCursor::setPos(widget->mapToGlobal(QPoint(mouseXDpiAdjusted, mouseYDpiAdjusted)));
    }

    Point2<int> MouseController::getMousePosition() const {
        QPoint mousePosition = widget->mapFromGlobal(QCursor::pos());
        //mouse coordinate computed in pixel
        return Point2<int>(
                MathFunction::roundToInt((float)mousePosition.x() * (float)widget->devicePixelRatio()),
                MathFunction::roundToInt((float)mousePosition.y() * (float)widget->devicePixelRatio()));
    }
}
