#include "CollisionSphereReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionSphereReaderWriter::loadFrom(const DataChunk* shapeChunk, const DataParser& dataParser) const {
        auto radiusChunk = dataParser.getUniqueChunk(true, RADIUS_TAG, UdaAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        return new CollisionSphereShape(radius);
    }

    void CollisionSphereReaderWriter::writeOn(DataChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaWriter& udaWriter) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, SPHERE_VALUE));

        const auto& sphereShape = dynamic_cast<const CollisionSphereShape&>(collisionShape);

        auto& radiusChunk = udaWriter.createChunk(RADIUS_TAG, UdaAttribute(), &shapeChunk);
        radiusChunk.setFloatValue(sphereShape.getRadius());
    }

}
