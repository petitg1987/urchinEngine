#pragma once

#include <UrchinPhysicsEngine.h>
#include <UrchinAggregation.h>

#include <panel/object/physics/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class BodyShapeWidgetRetriever {
        public:
            explicit BodyShapeWidgetRetriever(const ObjectEntity*);

            BodyShapeWidget* createBodyShapeWidget(CollisionShape3D::ShapeType) const;

        private:
            const ObjectEntity* objectEntity;
    };

}
