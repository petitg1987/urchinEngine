#pragma once

#include <QtWidgets/QPushButton>

namespace urchin {

    class ButtonStyleHelper {
        public:
            ButtonStyleHelper() = delete;

            static void applyNormalStyle(QPushButton*);
    };

}
