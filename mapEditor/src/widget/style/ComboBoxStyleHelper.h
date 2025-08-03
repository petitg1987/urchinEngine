#pragma once

#include <QtWidgets/QComboBox>

namespace urchin {

    class ComboBoxStyleHelper {
        public:
            static constexpr char EULER_XYZ_ORIENT_LABEL[] = "Euler XYZ";
            static constexpr char EULER_XZY_ORIENT_LABEL[] = "Euler XZY";
            static constexpr char EULER_YXZ_ORIENT_LABEL[] = "Euler YXZ";
            static constexpr char EULER_YZX_ORIENT_LABEL[] = "Euler YZX";
            static constexpr char EULER_ZXY_ORIENT_LABEL[] = "Euler ZXY";
            static constexpr char EULER_ZYX_ORIENT_LABEL[] = "Euler ZYX";
            static constexpr char EULER_XYX_ORIENT_LABEL[] = "Euler XYX";
            static constexpr char EULER_XZX_ORIENT_LABEL[] = "Euler XZX";
            static constexpr char EULER_YXY_ORIENT_LABEL[] = "Euler YXY";
            static constexpr char EULER_YZY_ORIENT_LABEL[] = "Euler YZY";
            static constexpr char EULER_ZXZ_ORIENT_LABEL[] = "Euler ZXZ";
            static constexpr char EULER_ZYZ_ORIENT_LABEL[] = "Euler ZYZ";

            ComboBoxStyleHelper() = delete;

            static void applyOrientationStyleOn(QComboBox*);
    };

}
