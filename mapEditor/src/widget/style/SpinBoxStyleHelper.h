#ifndef URCHINENGINE_SPINBOXSTYLEHELPER_H
#define URCHINENGINE_SPINBOXSTYLEHELPER_H

#include <QtWidgets/QDoubleSpinBox>
#include "UrchinCommon.h"

namespace urchin {

    class SpinBoxStyleHelper : public Singleton<SpinBoxStyleHelper> {
        public:
            friend class Singleton<SpinBoxStyleHelper>;

            void applyDefaultStyleOn(QDoubleSpinBox*);
            void applyAngleStyleOn(QDoubleSpinBox*);
            void applyPercentageStyleOn(QDoubleSpinBox*);

            void applyDefaultStyleOn(QSpinBox*);

        private:
            SpinBoxStyleHelper() = default;
            ~SpinBoxStyleHelper() override = default;
    };

}

#endif
