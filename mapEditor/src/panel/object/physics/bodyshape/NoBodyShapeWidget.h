#pragma once

#include <string>
#include <memory>
#include <UrchinPhysicsEngine.h>

#include "panel/object/physics/bodyshape/BodyShapeWidget.h"

namespace urchin {

    class NoBodyShapeWidget final : public BodyShapeWidget {
        Q_OBJECT

        public:
            explicit NoBodyShapeWidget(const ObjectEntity*);
            ~NoBodyShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(const CollisionShape3D&) override;
            std::unique_ptr<const CollisionShape3D> createBodyShape() const override;
    };

}
