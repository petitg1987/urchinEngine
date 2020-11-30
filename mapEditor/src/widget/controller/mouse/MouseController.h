#ifndef URCHINENGINE_MOUSECONTROLLER_H
#define URCHINENGINE_MOUSECONTROLLER_H

#include "UrchinCommon.h"
#include <QtWidgets/QWidget>

namespace urchin {

    class MouseController {
        public:
            MouseController(QWidget*);

            void moveMouse(unsigned int, unsigned int);
            Point2<int> getMousePosition() const;

        private:
            QWidget* widget;
    };

}

#endif
