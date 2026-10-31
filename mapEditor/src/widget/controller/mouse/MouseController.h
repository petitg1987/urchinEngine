#pragma once

#include <memory>
#include <optional>
#include <UrchinCommon.h>
#include <QtWidgets/QWidget>

#include "widget/controller/mouse/WaylandPointer.h"

namespace urchin {

    class MouseController {
        public:
            explicit MouseController(QWindow*);

            bool moveMouse(double, double) const;
            Point2<double> getMousePosition() const;

            bool consumeWarpEvent(double, double) const;
            bool takeRelativeMotion(double&, double&) const;
            void onMouseEnter() const;
            void onSurfaceAboutToBeDestroyed() const;

        private:
            QWindow* widget;
            std::unique_ptr<WaylandPointer> waylandPointer;
            mutable std::optional<Point2<double>> pendingWarpPosition;
    };

}
