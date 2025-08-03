#pragma once

#include <QtWidgets/QDoubleSpinBox>

namespace urchin {

    class SpinBoxStyleHelper {
        public:
            SpinBoxStyleHelper() = delete;

            static void applyDefaultStyle(QSpinBox*, int = -std::numeric_limits<int>::max(), int max = std::numeric_limits<int>::max(), int step = 1);

            static void applyDefaultStyle(QDoubleSpinBox*, double = -std::numeric_limits<double>::max(), double max = std::numeric_limits<double>::max(), double step = 0.1);
            static void applyAngleStyle(QDoubleSpinBox*);
            static void applyPercentageStyle(QDoubleSpinBox*);
    };

}
