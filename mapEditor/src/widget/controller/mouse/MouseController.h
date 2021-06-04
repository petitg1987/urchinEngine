#pragma once

#include <UrchinCommon.h>
#include <QtWidgets/QWidget>

namespace urchin {

    class MouseController {
        public:
            explicit MouseController(QWindow*);

            void moveMouse(unsigned int, unsigned int);
            Point2<unsigned int> getMousePosition() const;

        private:
            QWindow* widget;
    };

}
