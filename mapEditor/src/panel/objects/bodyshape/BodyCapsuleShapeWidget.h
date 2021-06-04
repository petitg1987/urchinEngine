#pragma once

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>

#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>
#include <panel/objects/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class BodyCapsuleShapeWidget : public BodyShapeWidget {
        Q_OBJECT

        public:
            explicit BodyCapsuleShapeWidget(const SceneObject*);
            ~BodyCapsuleShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(const std::shared_ptr<const CollisionShape3D>&) override;
            std::shared_ptr<const CollisionShape3D> createBodyShape() const override;

        private:
            QDoubleSpinBox* radius;
            QDoubleSpinBox* cylinderHeight;
            QComboBox* orientation;
    };

}
