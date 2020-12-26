#ifndef URCHINENGINE_FRAMESTYLEHELPER_H
#define URCHINENGINE_FRAMESTYLEHELPER_H

#include <QtWidgets/QFrame>

namespace urchin {

    class FrameStyleHelper {
        public:
            FrameStyleHelper() = delete;

            static void applyLineStyle(QFrame*);
    };

}

#endif
