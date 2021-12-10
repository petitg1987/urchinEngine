#pragma once

#include <memory>
#include <QWidget>
#include <UrchinPhysicsEngine.h>
#include <UrchinMapHandler.h>

#include <panel/objects/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class BodyShapeWidgetRetriever {
        public:
            explicit BodyShapeWidgetRetriever(const ObjectEntity*);

            std::unique_ptr<BodyShapeWidget> createBodyShapeWidget(CollisionShape3D::ShapeType);

        private:
            const ObjectEntity* objectEntity;
    };

}
