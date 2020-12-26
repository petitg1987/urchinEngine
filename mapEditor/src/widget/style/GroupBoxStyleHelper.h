#ifndef URCHINENGINE_GROUPBOXSTYLEHELPER_H
#define URCHINENGINE_GROUPBOXSTYLEHELPER_H

#include <QtWidgets/QGroupBox>

namespace urchin {

    class GroupBoxStyleHelper {
        public:
            GroupBoxStyleHelper() = delete;

            static void applyNormalStyle(QGroupBox*);
    };

}

#endif
