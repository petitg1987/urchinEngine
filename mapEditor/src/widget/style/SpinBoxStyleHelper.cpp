#include "widget/style/SpinBoxStyleHelper.h"

namespace urchin {


    void SpinBoxStyleHelper::applyDefaultStyleOn(QSpinBox* spinBox, int min, int max, int step) {
        spinBox->setMinimum(min);
        spinBox->setMaximum(max);
        spinBox->setSingleStep(step);
        spinBox->setKeyboardTracking(true);
        spinBox->setAccelerated(true);
        spinBox->setLocale(QLocale::English);
        spinBox->setFixedSize(QSize(80, 22));
    }

    void SpinBoxStyleHelper::applyDefaultStyleOn(QDoubleSpinBox* spinBox, double min, double max, double step) {
        spinBox->setDecimals(4);
        spinBox->setMinimum(min);
        spinBox->setMaximum(max);
        spinBox->setSingleStep(step);
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

}
