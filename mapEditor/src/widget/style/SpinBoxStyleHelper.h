#pragma once

#include <QtWidgets/QDoubleSpinBox>

namespace urchin {

    class SpinBoxStyleHelper {
        public:
            SpinBoxStyleHelper() = delete;

            static void applyDefaultStyleOn(QSpinBox*, int = -std::numeric_limits<int>::max(), int max = std::numeric_limits<int>::max(), int step = 1);

            static void applyDefaultStyleOn(QDoubleSpinBox*, double = -std::numeric_limits<double>::max(), double max = std::numeric_limits<double>::max(), double step = 0.1);
            static void applyAngleStyleOn(QDoubleSpinBox*);
            static void applyPercentageStyleOn(QDoubleSpinBox*);
    };

}
