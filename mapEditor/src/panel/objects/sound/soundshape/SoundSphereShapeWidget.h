#pragma once

#include <string>
#include <QtWidgets/QDoubleSpinBox>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>
#include <panel/objects/sound/soundshape/SoundShapeWidget.h>

namespace urchin {

    class SoundSphereShapeWidget final : public SoundShapeWidget {
        Q_OBJECT

        public:
            SoundSphereShapeWidget();
            ~SoundSphereShapeWidget() override = default;

            std::string getSoundShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(const SoundShape&) override;
            SoundShape* createSoundShape() const override;

        private:
            QDoubleSpinBox* radius;
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
    };

}
