#pragma once

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include <UrchinPhysicsEngine.h>
#include <panel/models/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class NoBodyShapeWidget : public BodyShapeWidget {
        Q_OBJECT

        public:
            explicit NoBodyShapeWidget(const SceneModel*);
            ~NoBodyShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(const CollisionShape3D&) override;
            std::unique_ptr<const CollisionShape3D> createBodyShape() const override;
    };

}
