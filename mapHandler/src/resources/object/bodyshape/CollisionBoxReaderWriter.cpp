#include "CollisionBoxReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionBoxReaderWriter::loadFrom(const XmlChunk* shapeChunk, const DataParser& dataParser) const {
        auto halfSizeChunk = dataParser.getUniqueChunk(true, HALF_SIZE_TAG, DataAttribute(), shapeChunk);
        Vector3<float> halfSize = halfSizeChunk->getVector3Value();

        return new CollisionBoxShape(halfSize);
    }

    void CollisionBoxReaderWriter::writeOn(XmlChunk* shapeChunk, const CollisionShape3D& collisionShape, XmlWriter& xmlWriter) const {
        shapeChunk->setAttribute(DataAttribute(TYPE_ATTR, BOX_VALUE));

        const auto& boxShape = dynamic_cast<const CollisionBoxShape&>(collisionShape);

        auto halfSizeChunk = xmlWriter.createChunk(HALF_SIZE_TAG, DataAttribute(), shapeChunk);
        halfSizeChunk->setVector3Value(boxShape.getHalfSizes());
    }

}
