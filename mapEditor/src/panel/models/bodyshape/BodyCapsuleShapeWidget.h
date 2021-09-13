#pragma once

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>

#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>
#include <panel/models/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class BodyCapsuleShapeWidget : public BodyShapeWidget {
        Q_OBJECT

        public:
            explicit BodyCapsuleShapeWidget(const SceneModel*);
            ~BodyCapsuleShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(const CollisionShape3D&) override;
            std::unique_ptr<const CollisionShape3D> createBodyShape() const override;

        private:
            QDoubleSpinBox* radius;
            QDoubleSpinBox* cylinderHeight;
            QComboBox* orientation;
    };

}
