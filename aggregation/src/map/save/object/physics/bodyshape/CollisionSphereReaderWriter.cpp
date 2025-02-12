#include <map/save/object/physics/bodyshape/CollisionSphereReaderWriter.h>

namespace urchin {

    std::unique_ptr<CollisionShape3D> CollisionSphereReaderWriter::load(const UdaChunk* shapeChunk, const UdaParser& udaParser) const {
        auto radiusChunk = udaParser.getFirstChunk(true, RADIUS_TAG, UdaAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        return std::make_unique<CollisionSphereShape>(radius);
    }

    void CollisionSphereReaderWriter::write(UdaChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaParser& udaParser) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, SPHERE_VALUE));

        const auto& sphereShape = static_cast<const CollisionSphereShape&>(collisionShape);

        auto& radiusChunk = udaParser.createChunk(RADIUS_TAG, UdaAttribute(), &shapeChunk);
        radiusChunk.setFloatValue(sphereShape.getRadius());
    }

}
