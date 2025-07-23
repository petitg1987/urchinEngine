#include "widget/style/SpinBoxStyleHelper.h"

namespace urchin {

    void SpinBoxStyleHelper::applyDefaultStyleOn(QDoubleSpinBox* spinBox) {
        spinBox->setDecimals(5);
        spinBox->setMinimum(-1e+09);
        spinBox->setMaximum(1e+09);
        spinBox->setSingleStep(0.1);
        spinBox->setKeyboardTracking(true);
        spinBox->setAccelerated(true);
        spinBox->setLocale(QLocale::English);
        spinBox->setFixedSize(QSize(80, 22));
    }

    void SpinBoxStyleHelper::applyAngleStyleOn(QDoubleSpinBox* spinBox) {
        spinBox->setDecimals(3);
        spinBox->setMinimum(-360);
        spinBox->setMaximum(360);
        spinBox->setSingleStep(5);
        spinBox->setKeyboardTracking(true);
        spinBox->setAccelerated(true);
        spinBox->setLocale(QLocale::English);
        spinBox->setFixedSize(QSize(80, 22));
    }

    void SpinBoxStyleHelper::applyPercentageStyleOn(QDoubleSpinBox* spinBox) {
        spinBox->setDecimals(0);
        spinBox->setMinimum(0);
        spinBox->setMaximum(100);
        spinBox->setSingleStep(5);
        spinBox->setKeyboardTracking(true);
        spinBox->setAccelerated(true);
        spinBox->setLocale(QLocale::English);
        spinBox->setFixedSize(QSize(80, 22));
    }

    void SpinBoxStyleHelper::applyDefaultStyleOn(QSpinBox* spinBox) {
        spinBox->setMinimum(-std::numeric_limits<int>::max());
        spinBox->setMaximum(std::numeric_limits<int>::max());
        spinBox->setSingleStep(1);
        spinBox->setKeyboardTracking(true);
        spinBox->setAccelerated(true);
        spinBox->setLocale(QLocale::English);
        spinBox->setFixedSize(QSize(80, 22));
    }

}
