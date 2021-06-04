#pragma once

#include <QtWidgets/QDoubleSpinBox>

namespace urchin {

    class SpinBoxStyleHelper {
        public:
            SpinBoxStyleHelper() = delete;

            static void applyDefaultStyleOn(QDoubleSpinBox*);
            static void applyAngleStyleOn(QDoubleSpinBox*);
            static void applyPercentageStyleOn(QDoubleSpinBox*);

            static void applyDefaultStyleOn(QSpinBox*);
    };

}
