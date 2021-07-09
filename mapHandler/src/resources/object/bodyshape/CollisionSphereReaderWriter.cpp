#include "CollisionSphereReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionSphereReaderWriter::loadFrom(const DataChunk* shapeChunk, const DataParser& dataParser) const {
        auto radiusChunk = dataParser.getUniqueChunk(true, RADIUS_TAG, DataAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        return new CollisionSphereShape(radius);
    }

    void CollisionSphereReaderWriter::writeOn(DataChunk* shapeChunk, const CollisionShape3D& collisionShape, DataWriter& dataWriter) const {
        shapeChunk->setAttribute(DataAttribute(TYPE_ATTR, SPHERE_VALUE));

        const auto& sphereShape = dynamic_cast<const CollisionSphereShape&>(collisionShape);

        auto radiusChunk = dataWriter.createChunk(RADIUS_TAG, DataAttribute(), shapeChunk);
        radiusChunk->setFloatValue(sphereShape.getRadius());
    }

}
