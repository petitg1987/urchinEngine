#include <stdexcept>

#include "CollisionCapsuleReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionCapsuleReaderWriter::loadFrom(const UdaChunk* shapeChunk, const DataParser& dataParser) const {
        auto orientationChunk = dataParser.getUniqueChunk(true, ORIENTATION_TAG, UdaAttribute(), shapeChunk);
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

        auto radiusChunk = dataParser.getUniqueChunk(true, RADIUS_TAG, UdaAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto cylinderHeightChunk = dataParser.getUniqueChunk(true, CYLINDER_HEIGHT_TAG, UdaAttribute(), shapeChunk);
        float cylinderHeight = cylinderHeightChunk->getFloatValue();

        return new CollisionCapsuleShape(radius, cylinderHeight, orientation);
    }

    void CollisionCapsuleReaderWriter::writeOn(UdaChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaWriter& udaWriter) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, CAPSULE_VALUE));

        const auto& capsuleShape = dynamic_cast<const CollisionCapsuleShape&>(collisionShape);

        auto& orientationChunk = udaWriter.createChunk(ORIENTATION_TAG, UdaAttribute(), &shapeChunk);
        CapsuleShape<float>::CapsuleOrientation orientationValue = capsuleShape.getCapsuleOrientation();
        if (orientationValue == CapsuleShape<float>::CapsuleOrientation::CAPSULE_X) {
            orientationChunk.setStringValue(X_VALUE);
        } else if (orientationValue == CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y) {
            orientationChunk.setStringValue(Y_VALUE);
        } else if (orientationValue == CapsuleShape<float>::CapsuleOrientation::CAPSULE_Z) {
            orientationChunk.setStringValue(Z_VALUE);
        } else {
            throw std::invalid_argument("Capsule orientation type unknown: " + std::to_string(orientationValue));
        }

        auto& radiusChunk = udaWriter.createChunk(RADIUS_TAG, UdaAttribute(), &shapeChunk);
        radiusChunk.setFloatValue(capsuleShape.getRadius());

        auto& cylinderHeightChunk = udaWriter.createChunk(CYLINDER_HEIGHT_TAG, UdaAttribute(), &shapeChunk);
        cylinderHeightChunk.setFloatValue(capsuleShape.getCylinderHeight());
    }

}
