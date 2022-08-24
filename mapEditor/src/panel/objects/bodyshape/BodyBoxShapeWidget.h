#pragma once

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <panel/objects/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class BodyBoxShapeWidget final : public BodyShapeWidget {
        Q_OBJECT

        public:
            explicit BodyBoxShapeWidget(const ObjectEntity*);
            ~BodyBoxShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(const CollisionShape3D&) override;
            std::unique_ptr<const CollisionShape3D> createBodyShape() const override;

        private:
            QDoubleSpinBox* halfSizeX;
            QDoubleSpinBox* halfSizeY;
            QDoubleSpinBox* halfSizeZ;
    };

}
