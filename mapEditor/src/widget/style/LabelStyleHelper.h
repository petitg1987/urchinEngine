#ifndef URCHINENGINE_LABELSTYLEHELPER_H
#define URCHINENGINE_LABELSTYLEHELPER_H

#include <string>
#include <QtWidgets/QLabel>
#include "UrchinCommon.h"

namespace urchin {

    class LabelStyleHelper : public Singleton<LabelStyleHelper> {
        public:
            friend class Singleton<LabelStyleHelper>;

            void applyErrorStyle(QLabel*, const std::string&);
            void applyNormalStyle(QLabel*);

        private:
            LabelStyleHelper() = default;
            ~LabelStyleHelper() override = default;
    };

}

#endif
