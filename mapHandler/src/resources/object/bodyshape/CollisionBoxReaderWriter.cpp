#include <resources/object/bodyshape/CollisionBoxReaderWriter.h>

namespace urchin {

    CollisionShape3D* CollisionBoxReaderWriter::loadFrom(const UdaChunk* shapeChunk, const UdaParser& udaParser) const {
        auto halfSizeChunk = udaParser.getUniqueChunk(true, HALF_SIZE_TAG, UdaAttribute(), shapeChunk);
        Vector3<float> halfSize = halfSizeChunk->getVector3Value();

        return new CollisionBoxShape(halfSize);
    }

    void CollisionBoxReaderWriter::writeOn(UdaChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaWriter& udaWriter) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, BOX_VALUE));

        const auto& boxShape = static_cast<const CollisionBoxShape&>(collisionShape);

        auto& halfSizeChunk = udaWriter.createChunk(HALF_SIZE_TAG, UdaAttribute(), &shapeChunk);
        halfSizeChunk.setVector3Value(boxShape.getHalfSizes());
    }

}
