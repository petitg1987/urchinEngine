#pragma once

#include <UrchinCommon.h>
#include <QtWidgets/QWidget>

namespace urchin {

    class MouseController {
        public:
            explicit MouseController(QWindow*);

            void moveMouse(double, double) const;
            Point2<double> getMousePosition() const;

        private:
            QWindow* widget;
    };

}
