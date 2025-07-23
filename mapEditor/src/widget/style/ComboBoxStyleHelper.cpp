#include <UrchinCommon.h>

#include "widget/style/ComboBoxStyleHelper.h"

namespace urchin {

    void ComboBoxStyleHelper::applyDefaultStyleOn(QComboBox* comboBox) {
        comboBox->setLocale(QLocale::English);
    }

    void ComboBoxStyleHelper::applyOrientationStyleOn(QComboBox* comboBox) {
        comboBox->addItem(EULER_XYZ_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::XYZ));
        comboBox->addItem(EULER_XZY_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::XZY));
        comboBox->addItem(EULER_YXZ_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::YXZ));
        comboBox->addItem(EULER_YZX_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::YZX));
        comboBox->addItem(EULER_ZXY_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::ZXY));
        comboBox->addItem(EULER_ZYX_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::ZYX));
        comboBox->addItem(EULER_XYX_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::XYX));
        comboBox->addItem(EULER_XZX_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::XZX));
        comboBox->addItem(EULER_YXY_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::YXY));
        comboBox->addItem(EULER_YZY_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::YZY));
        comboBox->addItem(EULER_ZXZ_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::ZXZ));
        comboBox->addItem(EULER_ZYZ_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::ZYZ));

        comboBox->setLocale(QLocale::English);
    }

}
