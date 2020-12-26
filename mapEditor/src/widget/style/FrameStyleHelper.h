#ifndef URCHINENGINE_FRAMESTYLEHELPER_H
#define URCHINENGINE_FRAMESTYLEHELPER_H

#include <QtWidgets/QFrame>
#include "UrchinCommon.h"

namespace urchin {

    class FrameStyleHelper : public Singleton<FrameStyleHelper> {
        public:
            friend class Singleton<FrameStyleHelper>;

            void applyLineStyle(QFrame*);

        private:
            FrameStyleHelper() = default;
            ~FrameStyleHelper() override = default;
    };

}

#endif
