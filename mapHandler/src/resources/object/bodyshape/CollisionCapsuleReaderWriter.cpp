#include <stdexcept>

#include "CollisionCapsuleReaderWriter.h"

namespace urchin {

    CollisionShape3D *CollisionCapsuleReaderWriter::loadFrom(const std::shared_ptr<XmlChunk> &shapeChunk, const XmlParser &xmlParser) const {
        std::shared_ptr<XmlChunk> orientationChunk = xmlParser.getUniqueChunk(true, ORIENTATION_TAG, XmlAttribute(), shapeChunk);
        std::string orientationValue = orientationChunk->getStringValue();
        CapsuleShape<float>::CapsuleOrientation orientation;
        if (orientationValue == X_VALUE) {
            orientation = CapsuleShape<float>::CapsuleOrientation::CAPSULE_X;
        } else if (orientationValue == Y_VALUE) {
            orientation = CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y;
        } else if (orientationValue == Z_VALUE) {
            orientation = CapsuleShape<float>::CapsuleOrientation::CAPSULE_Z;
        } else {
            throw std::invalid_argument("Capsule orientation type unknown: " + orientationValue);
        }

        std::shared_ptr<XmlChunk> radiusChunk = xmlParser.getUniqueChunk(true, RADIUS_TAG, XmlAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        std::shared_ptr<XmlChunk> cylinderHeightChunk = xmlParser.getUniqueChunk(true, CYLINDER_HEIGHT_TAG, XmlAttribute(), shapeChunk);
        float cylinderHeight = cylinderHeightChunk->getFloatValue();

        return new CollisionCapsuleShape(radius, cylinderHeight, orientation);
    }

    void CollisionCapsuleReaderWriter::writeOn(const std::shared_ptr<XmlChunk> &shapeChunk, const CollisionShape3D *collisionShape, XmlWriter &xmlWriter) const {
        shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, CAPSULE_VALUE));

        const auto *capsuleShape = dynamic_cast<const CollisionCapsuleShape *>(collisionShape);

        std::shared_ptr<XmlChunk> orientationChunk = xmlWriter.createChunk(ORIENTATION_TAG, XmlAttribute(), shapeChunk);
        CapsuleShape<float>::CapsuleOrientation orientationValue = capsuleShape->getCapsuleOrientation();
        if (orientationValue==CapsuleShape<float>::CapsuleOrientation::CAPSULE_X) {
            orientationChunk->setStringValue(X_VALUE);
        } else if (orientationValue==CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y) {
            orientationChunk->setStringValue(Y_VALUE);
        } else if (orientationValue==CapsuleShape<float>::CapsuleOrientation::CAPSULE_Z) {
            orientationChunk->setStringValue(Z_VALUE);
        } else {
            throw std::invalid_argument("Capsule orientation type unknown: " + std::to_string(orientationValue));
        }

        std::shared_ptr<XmlChunk> radiusChunk = xmlWriter.createChunk(RADIUS_TAG, XmlAttribute(), shapeChunk);
        radiusChunk->setFloatValue(capsuleShape->getRadius());

        std::shared_ptr<XmlChunk> cylinderHeightChunk = xmlWriter.createChunk(CYLINDER_HEIGHT_TAG, XmlAttribute(), shapeChunk);
        cylinderHeightChunk->setFloatValue(capsuleShape->getCylinderHeight());
    }

}
