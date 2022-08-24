#pragma once

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>
#include <panel/sounds/soundshape/SoundShapeWidget.h>

namespace urchin {

    class SoundBoxShapeWidget final : public SoundShapeWidget {
        Q_OBJECT

        public:
            explicit SoundBoxShapeWidget(const SoundEntity&);
            ~SoundBoxShapeWidget() override = default;

            std::string getSoundShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(const SoundShape&) override;
            SoundShape* createSoundShape() const override;

        private:
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QDoubleSpinBox* halfSizeX;
            QDoubleSpinBox* halfSizeY;
            QDoubleSpinBox* halfSizeZ;
            QComboBox* orientationType;
            QDoubleSpinBox* eulerAxis0;
            QDoubleSpinBox* eulerAxis1;
            QDoubleSpinBox* eulerAxis2;

        private slots:
            void updateShapeOrientationType();
    };

}
