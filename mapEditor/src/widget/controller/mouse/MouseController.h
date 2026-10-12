#pragma once

#include <UrchinCommon.h>
#include <QtWidgets/QWidget>

namespace urchin {

    class MouseController {
        public:
            explicit MouseController(QWindow*);

            bool moveMouse(double, double) const;
            Point2<double> getMousePosition() const;

        private:
            QWindow* widget;
    };

}
