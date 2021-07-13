#include <stdexcept>

#include "CollisionCapsuleReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionCapsuleReaderWriter::loadFrom(const DataChunk* shapeChunk, const DataParser& dataParser) const {
        auto orientationChunk = dataParser.getUniqueChunk(true, ORIENTATION_TAG, DataAttribute(), shapeChunk);
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

        auto radiusChunk = dataParser.getUniqueChunk(true, RADIUS_TAG, DataAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto cylinderHeightChunk = dataParser.getUniqueChunk(true, CYLINDER_HEIGHT_TAG, DataAttribute(), shapeChunk);
        float cylinderHeight = cylinderHeightChunk->getFloatValue();

        return new CollisionCapsuleShape(radius, cylinderHeight, orientation);
    }

    void CollisionCapsuleReaderWriter::writeOn(DataChunk& shapeChunk, const CollisionShape3D& collisionShape, DataWriter& dataWriter) const {
        shapeChunk.addAttribute(DataAttribute(TYPE_ATTR, CAPSULE_VALUE));

        const auto& capsuleShape = dynamic_cast<const CollisionCapsuleShape&>(collisionShape);

        auto& orientationChunk = dataWriter.createChunk(ORIENTATION_TAG, DataAttribute(), &shapeChunk);
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

        auto& radiusChunk = dataWriter.createChunk(RADIUS_TAG, DataAttribute(), &shapeChunk);
        radiusChunk.setFloatValue(capsuleShape.getRadius());

        auto& cylinderHeightChunk = dataWriter.createChunk(CYLINDER_HEIGHT_TAG, DataAttribute(), &shapeChunk);
        cylinderHeightChunk.setFloatValue(capsuleShape.getCylinderHeight());
    }

}
