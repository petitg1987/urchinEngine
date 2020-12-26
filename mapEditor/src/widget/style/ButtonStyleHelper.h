#ifndef URCHINENGINE_BUTTONSTYLEHELPER_H
#define URCHINENGINE_BUTTONSTYLEHELPER_H

#include <QtWidgets/QPushButton>

namespace urchin {

    class ButtonStyleHelper {
        public:
            ButtonStyleHelper() = delete;

            static void applyNormalStyle(QPushButton*);
    };

}

#endif
