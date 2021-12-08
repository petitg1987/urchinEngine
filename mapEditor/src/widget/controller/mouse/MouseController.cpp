#include <widget/controller/mouse/MouseController.h>
#include <scene/SceneDisplayerWindow.h>

namespace urchin {

    MouseController::MouseController(QWindow* widget) :
            widget(widget) {

    }

    void MouseController::moveMouse(double x, double y) {
        int mouseXDpiAdjusted = MathFunction::roundToInt(x / (double)widget->devicePixelRatio());
        int mouseYDpiAdjusted = MathFunction::roundToInt(y / (double)widget->devicePixelRatio());
        QCursor::setPos(widget->mapToGlobal(QPoint(mouseXDpiAdjusted, mouseYDpiAdjusted)));
    }

    Point2<double> MouseController::getMousePosition() const {
        QPoint mousePosition = widget->mapFromGlobal(QCursor::pos());
        //mouse coordinate computed in pixel
        return Point2<double>(
                (double)mousePosition.x() * (double)widget->devicePixelRatio(),
                (double)mousePosition.y() * (double)widget->devicePixelRatio());
    }
}
