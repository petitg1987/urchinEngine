#include <stdexcept>

#include <resources/object/bodyshape/CollisionConeReaderWriter.h>

namespace urchin {

    CollisionShape3D* CollisionConeReaderWriter::loadFrom(const UdaChunk* shapeChunk, const UdaParser& udaParser) const {
        auto orientationChunk = udaParser.getUniqueChunk(true, ORIENTATION_TAG, UdaAttribute(), shapeChunk);
        std::string orientationValue = orientationChunk->getStringValue();
        ConeShape<float>::ConeOrientation orientation;
        if (orientationValue == X_POSITIVE_VALUE) {
            orientation = ConeShape<float>::ConeOrientation::CONE_X_POSITIVE;
        } else if (orientationValue == X_NEGATIVE_VALUE) {
            orientation = ConeShape<float>::ConeOrientation::CONE_X_NEGATIVE;
        } else if (orientationValue == Y_POSITIVE_VALUE) {
            orientation = ConeShape<float>::ConeOrientation::CONE_Y_POSITIVE;
        } else if (orientationValue == Y_NEGATIVE_VALUE) {
            orientation = ConeShape<float>::ConeOrientation::CONE_Y_NEGATIVE;
        } else if (orientationValue == Z_POSITIVE_VALUE) {
            orientation = ConeShape<float>::ConeOrientation::CONE_Z_POSITIVE;
        } else if (orientationValue == Z_NEGATIVE_VALUE) {
            orientation = ConeShape<float>::ConeOrientation::CONE_Z_NEGATIVE;
        } else {
            throw std::invalid_argument("Cone orientation type unknown: " + orientationValue);
        }

        auto radiusChunk = udaParser.getUniqueChunk(true, RADIUS_TAG, UdaAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto heightChunk = udaParser.getUniqueChunk(true, HEIGHT_TAG, UdaAttribute(), shapeChunk);
        float height = heightChunk->getFloatValue();

        return new CollisionConeShape(radius, height, orientation);
    }

    void CollisionConeReaderWriter::writeOn(UdaChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaWriter& udaWriter) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, CONE_VALUE));

        const auto& coneShape = dynamic_cast<const CollisionConeShape&>(collisionShape);

        auto& orientationChunk = udaWriter.createChunk(ORIENTATION_TAG, UdaAttribute(), &shapeChunk);
        ConeShape<float>::ConeOrientation orientationValue = coneShape.getConeOrientation();
        if (orientationValue == ConeShape<float>::ConeOrientation::CONE_X_POSITIVE) {
            orientationChunk.setStringValue(X_POSITIVE_VALUE);
        } else if (orientationValue == ConeShape<float>::ConeOrientation::CONE_X_NEGATIVE) {
            orientationChunk.setStringValue(X_NEGATIVE_VALUE);
        } else if (orientationValue == ConeShape<float>::ConeOrientation::CONE_Y_POSITIVE) {
            orientationChunk.setStringValue(Y_POSITIVE_VALUE);
        } else if (orientationValue == ConeShape<float>::ConeOrientation::CONE_Y_NEGATIVE) {
            orientationChunk.setStringValue(Y_NEGATIVE_VALUE);
        } else if (orientationValue == ConeShape<float>::ConeOrientation::CONE_Z_POSITIVE) {
            orientationChunk.setStringValue(Z_POSITIVE_VALUE);
        } else if (orientationValue == ConeShape<float>::ConeOrientation::CONE_Z_NEGATIVE) {
            orientationChunk.setStringValue(Z_NEGATIVE_VALUE);
        } else {
            throw std::invalid_argument("Cone orientation type unknown: " + std::to_string(orientationValue));
        }

        auto& radiusChunk = udaWriter.createChunk(RADIUS_TAG, UdaAttribute(), &shapeChunk);
        radiusChunk.setFloatValue(coneShape.getRadius());

        auto& heightChunk = udaWriter.createChunk(HEIGHT_TAG, UdaAttribute(), &shapeChunk);
        heightChunk.setFloatValue(coneShape.getHeight());
    }

}
