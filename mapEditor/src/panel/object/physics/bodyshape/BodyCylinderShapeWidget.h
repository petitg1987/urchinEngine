#pragma once

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <panel/object/physics/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class BodyCylinderShapeWidget final : public BodyShapeWidget {
        Q_OBJECT

        public:
            explicit BodyCylinderShapeWidget(const ObjectEntity*);
            ~BodyCylinderShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        public slots:
            void updateCylinderOrientation();

        protected:
            void doSetupShapePropertiesFrom(const CollisionShape3D&) override;
            std::unique_ptr<const CollisionShape3D> createBodyShape() const override;

        private:
            void disableShapeEvents(bool) const;

            QDoubleSpinBox* radius;
            QDoubleSpinBox* height;
            QComboBox* orientation;
    };

}
