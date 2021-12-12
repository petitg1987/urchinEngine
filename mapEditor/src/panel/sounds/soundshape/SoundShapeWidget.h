#pragma once

#include <string>
#include <memory>
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

            explicit SoundShapeWidget(const SoundEntity&);
            ~SoundShapeWidget() override = default;

            const SoundEntity& getSoundEntity() const;
            virtual std::string getSoundShapeName() const = 0;
            const SoundShape* retrieveShape();

            void setupShapePropertiesFrom(const SoundShape&);

        signals:
            void soundShapeChange(const SoundShape*);

        public slots:
            void updateSoundShape();

        protected:
            QGridLayout* mainLayout;
            bool disableShapeEvent;

            float getMarginValue() const;

            virtual void doSetupShapePropertiesFrom(const SoundShape&) = 0;
            virtual const SoundShape* createSoundShape() const = 0;

        private:
            QDoubleSpinBox* margin;

            const SoundEntity& soundEntity;
            const SoundShape* shape;
    };

}
