#pragma once

#include <QWidget>
#include <UrchinPhysicsEngine.h>
#include <UrchinAggregation.h>

#include <panel/objects/physics/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class BodyShapeWidgetRetriever {
        public:
            explicit BodyShapeWidgetRetriever(const ObjectEntity*);

            BodyShapeWidget* createBodyShapeWidget(CollisionShape3D::ShapeType) const;

        private:
            const ObjectEntity* objectEntity;
    };

}
