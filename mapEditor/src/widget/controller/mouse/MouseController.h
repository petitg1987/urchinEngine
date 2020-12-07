#ifndef URCHINENGINE_MOUSECONTROLLER_H
#define URCHINENGINE_MOUSECONTROLLER_H

#include "UrchinCommon.h"
#include <QtWidgets/QWidget>

namespace urchin {

    class MouseController {
        public:
            explicit MouseController(QWidget*);

            void moveMouse(unsigned int, unsigned int);
            Point2<unsigned int> getMousePosition() const;

        private:
            QWidget* widget;
    };

}

#endif
