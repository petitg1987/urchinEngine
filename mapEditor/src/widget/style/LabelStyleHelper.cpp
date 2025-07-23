#include "widget/style/LabelStyleHelper.h"

namespace urchin {

    void LabelStyleHelper::applyErrorStyle(QLabel* label, const std::string& errorMsg) {
        label->setStyleSheet("QLabel {color : red; }");
        label->setToolTip(QString::fromStdString(errorMsg));
    }

    void LabelStyleHelper::applyNormalStyle(QLabel* label) {
        label->setStyleSheet("QLabel {}");
        label->setToolTip("");
    }

}
