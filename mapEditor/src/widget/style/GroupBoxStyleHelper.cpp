#include <widget/style/GroupBoxStyleHelper.h>

namespace urchin {

    void GroupBoxStyleHelper::applyNormalStyle(QGroupBox* groupBox) {
        groupBox->setStyleSheet("QGroupBox {border: 1px solid gray;border-radius: 9px;margin-top: 0.5em;} "
                        "QGroupBox::title {subcontrol-origin: margin;left: 10px;padding: 0 0px 0 0px;}");
    }

}
