#ifndef URCHINENGINE_BUTTONSTYLEHELPER_H
#define URCHINENGINE_BUTTONSTYLEHELPER_H

#include <QtWidgets/QPushButton>
#include "UrchinCommon.h"

namespace urchin {

    class ButtonStyleHelper : public Singleton<ButtonStyleHelper> {
        public:
            friend class Singleton<ButtonStyleHelper>;

            void applyNormalStyle(QPushButton*);

        private:
            ButtonStyleHelper() = default;
            ~ButtonStyleHelper() override = default;
    };

}

#endif
