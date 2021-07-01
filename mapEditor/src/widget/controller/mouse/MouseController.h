#pragma once

#include <UrchinCommon.h>
#include <QtWidgets/QWidget>

namespace urchin {

    class MouseController {
        public:
            explicit MouseController(QWindow*);

            void moveMouse(int, int);
            Point2<int> getMousePosition() const;

        private:
            QWindow* widget;
    };

}
