#pragma once

#include <QtWidgets/QGroupBox>

namespace urchin {

    class GroupBoxStyleHelper {
        public:
            GroupBoxStyleHelper() = delete;

            static void applyDefaultStyle(QGroupBox*);
    };

}
