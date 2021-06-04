#pragma once

#include <QtWidgets/QFrame>

namespace urchin {

    class FrameStyleHelper {
        public:
            FrameStyleHelper() = delete;

            static void applyLineStyle(QFrame*);
    };

}
