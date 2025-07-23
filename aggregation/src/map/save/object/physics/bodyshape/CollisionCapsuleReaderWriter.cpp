#include <stdexcept>

#include "map/save/object/physics/bodyshape/CollisionCapsuleReaderWriter.h"

namespace urchin {

    std::unique_ptr<CollisionShape3D> CollisionCapsuleReaderWriter::load(const UdaChunk* shapeChunk, const UdaParser& udaParser) const {
        auto orientationChunk = udaParser.getFirstChunk(true, ORIENTATION_TAG, UdaAttribute(), shapeChunk);
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

        auto radiusChunk = udaParser.getFirstChunk(true, RADIUS_TAG, UdaAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto cylinderHeightChunk = udaParser.getFirstChunk(true, CYLINDER_HEIGHT_TAG, UdaAttribute(), shapeChunk);
        float cylinderHeight = cylinderHeightChunk->getFloatValue();

        return std::make_unique<CollisionCapsuleShape>(radius, cylinderHeight, orientation);
    }

    void CollisionCapsuleReaderWriter::write(UdaChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaParser& udaParser) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, CAPSULE_VALUE));

        const auto& capsuleShape = static_cast<const CollisionCapsuleShape&>(collisionShape);

        auto& orientationChunk = udaParser.createChunk(ORIENTATION_TAG, UdaAttribute(), &shapeChunk);
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

        auto& radiusChunk = udaParser.createChunk(RADIUS_TAG, UdaAttribute(), &shapeChunk);
        radiusChunk.setFloatValue(capsuleShape.getRadius());

        auto& cylinderHeightChunk = udaParser.createChunk(CYLINDER_HEIGHT_TAG, UdaAttribute(), &shapeChunk);
        cylinderHeightChunk.setFloatValue(capsuleShape.getCylinderHeight());
    }

}
