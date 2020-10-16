#ifndef URCHINENGINE_SPINBOXSTYLEHELPER_H
#define URCHINENGINE_SPINBOXSTYLEHELPER_H

#include <QtWidgets/QDoubleSpinBox>

namespace urchin {

    class SpinBoxStyleHelper {
        public:
            static void applyDefaultStyleOn(QDoubleSpinBox *);
            static void applyAngleStyleOn(QDoubleSpinBox *);
            static void applyPercentageStyleOn(QDoubleSpinBox *);

            static void applyDefaultStyleOn(QSpinBox *);
    };

}

#endif
