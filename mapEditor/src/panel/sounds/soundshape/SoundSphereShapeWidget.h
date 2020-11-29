#ifndef URCHINENGINE_SOUNDSPHERESHAPEWIDGET_H
#define URCHINENGINE_SOUNDSPHERESHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include "UrchinCommon.h"
#include "UrchinSoundEngine.h"
#include "panel/sounds/soundshape/SoundShapeWidget.h"

namespace urchin {

    class SoundSphereShapeWidget : public SoundShapeWidget {
        Q_OBJECT

        public:
            explicit SoundSphereShapeWidget(const SceneSound*);
            ~SoundSphereShapeWidget() override = default;

            std::string getSoundShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(const SoundShape*) override;
            const SoundShape* createSoundShape() const override;

        private:
            QDoubleSpinBox* radius;
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
    };

}

#endif
