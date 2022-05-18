#include <map/save/object/bodyshape/CollisionBoxReaderWriter.h>

namespace urchin {

    std::unique_ptr<CollisionShape3D> CollisionBoxReaderWriter::load(const UdaChunk* shapeChunk, const UdaParser& udaParser) const {
        auto halfSizeChunk = udaParser.getFirstChunk(true, HALF_SIZE_TAG, UdaAttribute(), shapeChunk);
        Vector3<float> halfSize = halfSizeChunk->getVector3Value();

        return std::make_unique<CollisionBoxShape>(halfSize);
    }

    void CollisionBoxReaderWriter::write(UdaChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaParser& udaParser) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, BOX_VALUE));

        const auto& boxShape = static_cast<const CollisionBoxShape&>(collisionShape);

        auto& halfSizeChunk = udaParser.createChunk(HALF_SIZE_TAG, UdaAttribute(), &shapeChunk);
        halfSizeChunk.setVector3Value(boxShape.getHalfSizes());
    }

}
