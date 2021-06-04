#pragma once

#include <string>
#include <QtWidgets/QLabel>

namespace urchin {

    class LabelStyleHelper {
        public:
            LabelStyleHelper() = delete;

            static void applyErrorStyle(QLabel*, const std::string&);
            static void applyNormalStyle(QLabel*);
    };

}
