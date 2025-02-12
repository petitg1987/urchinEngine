#pragma once

#include <string>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>

#include <UrchinCommon.h>
#include <UrchinAggregation.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundShapeWidget : public QWidget {
        Q_OBJECT

        public:
            static constexpr char SPHERE_SHAPE_LABEL[] = "Sphere";
            static constexpr char BOX_SHAPE_LABEL[] = "Box";

            SoundShapeWidget(const ObjectEntity&);
            ~SoundShapeWidget() override = default;

            virtual std::string getSoundShapeName() const = 0;
            const SoundShape* retrieveShape();

            void setupShapePropertiesFrom(const SoundShape&);

        signals:
            void soundShapeChange(SoundShape*);

        public slots:
            void updateSoundShape();

        protected:
            QGridLayout* mainLayout;
            bool disableShapeEvent;

            const ObjectEntity& getObjectEntity() const;
            float getMarginValue() const;

            virtual void doSetupShapePropertiesFrom(const SoundShape&) = 0;
            virtual SoundShape* createSoundShape() const = 0;

        private:
            const ObjectEntity& objectEntity;
            QDoubleSpinBox* margin;

            SoundShape* shape;
    };

}
