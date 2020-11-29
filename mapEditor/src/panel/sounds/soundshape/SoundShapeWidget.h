#ifndef URCHINENGINE_SOUNDSHAPEWIDGET_H
#define URCHINENGINE_SOUNDSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "UrchinSoundEngine.h"

namespace urchin {

    class SoundShapeWidget : public QWidget {
        Q_OBJECT

        #define SPHERE_SHAPE_LABEL "Sphere"
        #define BOX_SHAPE_LABEL "Box"

        public:
            explicit SoundShapeWidget(const SceneSound*);
            ~SoundShapeWidget() override = default;

            const SceneSound* getSceneSound() const;
            virtual std::string getSoundShapeName() const = 0;
            const SoundShape* retrieveShape();

            void setupShapePropertiesFrom(const SoundShape*);

        signals:
            void soundShapeChange(const SoundShape*);

        public slots:
            void updateSoundShape();

        protected:
            QGridLayout *mainLayout;
            bool disableShapeEvent;

            float getMarginValue() const;

            virtual void doSetupShapePropertiesFrom(const SoundShape*) = 0;
            virtual const SoundShape* createSoundShape() const = 0;

        private:
            QDoubleSpinBox *margin;

            const SceneSound *sceneSound;
            const SoundShape *shape;
    };

}

#endif
