#include "widget/controller/mouse/MouseController.h"
#include "scene/SceneDisplayerWindow.h"
#include <QtGui/QGuiApplication>

namespace urchin {

    MouseController::MouseController(QWindow* widget) :
            widget(widget) {

    }

    bool MouseController::moveMouse(double x, double y) const {
        if (QGuiApplication::platformName() == "wayland") {
            return false; //Wayland doesn't allow cursor positioning
        }
        int mouseXDpiAdjusted = MathFunction::roundToInt(x / widget->devicePixelRatio());
        int mouseYDpiAdjusted = MathFunction::roundToInt(y / widget->devicePixelRatio());
        QCursor::setPos(widget->mapToGlobal(QPoint(mouseXDpiAdjusted, mouseYDpiAdjusted)));
        return true;
    }

    Point2<double> MouseController::getMousePosition() const {
        QPoint mousePosition = widget->mapFromGlobal(QCursor::pos());
        //mouse coordinate computed in pixel
        return Point2(
                (double)mousePosition.x() * widget->devicePixelRatio(),
                (double)mousePosition.y() * widget->devicePixelRatio());
    }
}
