#ifndef URCHINENGINE_COMBOBOXSTYLEHELPER_H
#define URCHINENGINE_COMBOBOXSTYLEHELPER_H

#include <QtWidgets/QComboBox>

namespace urchin {

    class ComboBoxStyleHelper {
        #define EULER_XYZ_ORIENT_LABEL "Euler XYZ"
        #define EULER_XZY_ORIENT_LABEL "Euler XZY"
        #define EULER_YXZ_ORIENT_LABEL "Euler YXZ"
        #define EULER_YZX_ORIENT_LABEL "Euler YZX"
        #define EULER_ZXY_ORIENT_LABEL "Euler ZXY"
        #define EULER_ZYX_ORIENT_LABEL "Euler ZYX"
        #define EULER_XYX_ORIENT_LABEL "Euler XYX"
        #define EULER_XZX_ORIENT_LABEL "Euler XZX"
        #define EULER_YXY_ORIENT_LABEL "Euler YXY"
        #define EULER_YZY_ORIENT_LABEL "Euler YZY"
        #define EULER_ZXZ_ORIENT_LABEL "Euler ZXZ"
        #define EULER_ZYZ_ORIENT_LABEL "Euler ZYZ"

        public:
            static void applyDefaultStyleOn(QComboBox *);
            static void applyOrientationStyleOn(QComboBox *);
    };

}

#endif
