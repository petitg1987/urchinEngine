#include "widget/controller/mouse/MouseController.h"
#include "scene/SceneDisplayerWindow.h"
#include <QtGui/QGuiApplication>

namespace urchin {

    MouseController::MouseController(QWindow* widget) :
            widget(widget) {
        if (QGuiApplication::platformName() == "wayland") {
            waylandPointer = std::make_unique<WaylandPointer>();
        }
    }

    bool MouseController::moveMouse(double x, double y) const {
        if (waylandPointer) {
            return waylandPointer->warp(x / widget->devicePixelRatio(), y / widget->devicePixelRatio());
        }

        int mouseXDpiAdjusted = MathFunction::roundToInt(x / widget->devicePixelRatio());
        int mouseYDpiAdjusted = MathFunction::roundToInt(y / widget->devicePixelRatio());
        QCursor::setPos(widget->mapToGlobal(QPoint(mouseXDpiAdjusted, mouseYDpiAdjusted)));
        pendingWarpPosition = Point2((double)mouseXDpiAdjusted * widget->devicePixelRatio(), (double)mouseYDpiAdjusted * widget->devicePixelRatio());
        return true;
    }

    bool MouseController::consumeWarpEvent(double x, double y) const {
        constexpr double POSITION_TOLERANCE = 1.0;
        if (pendingWarpPosition && std::abs(x - pendingWarpPosition->X) <= POSITION_TOLERANCE && std::abs(y - pendingWarpPosition->Y) <= POSITION_TOLERANCE) {
            pendingWarpPosition.reset();
            return true;
        }
        return false;
    }

    bool MouseController::takeRelativeMotion(double& deltaX, double& deltaY) const {
        if (!waylandPointer || !waylandPointer->takeRelativeMotion(deltaX, deltaY)) {
            return false;
        }
        deltaX *= widget->devicePixelRatio();
        deltaY *= widget->devicePixelRatio();
        return true;
    }

    void MouseController::onMouseEnter() const {
        if (waylandPointer) {
            waylandPointer->captureWindowSurface();
            waylandPointer->resetRelativeMotion(); //ignore mouse movements done outside the window
        }
    }

    void MouseController::onSurfaceAboutToBeDestroyed() const {
        if (waylandPointer) {
            waylandPointer->releaseWindowSurface();
        }
    }

    Point2<double> MouseController::getMousePosition() const {
        QPoint mousePosition = widget->mapFromGlobal(QCursor::pos());
        //mouse coordinate computed in pixel
        return Point2(
                (double)mousePosition.x() * widget->devicePixelRatio(),
                (double)mousePosition.y() * widget->devicePixelRatio()
        );
    }
}
