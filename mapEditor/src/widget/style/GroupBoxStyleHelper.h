#ifndef URCHINENGINE_GROUPBOXSTYLEHELPER_H
#define URCHINENGINE_GROUPBOXSTYLEHELPER_H

#include <QtWidgets/QGroupBox>
#include "UrchinCommon.h"

namespace urchin {

    class GroupBoxStyleHelper : public Singleton<GroupBoxStyleHelper> {
        public:
            friend class Singleton<GroupBoxStyleHelper>;

            void applyNormalStyle(QGroupBox*);

        private:
            GroupBoxStyleHelper() = default;
            ~GroupBoxStyleHelper() override = default;
    };

}

#endif
