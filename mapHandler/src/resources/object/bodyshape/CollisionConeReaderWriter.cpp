#include <stdexcept>

#include "CollisionConeReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionConeReaderWriter::loadFrom(const XmlChunk* shapeChunk, const DataParser& dataParser) const {
        auto orientationChunk = dataParser.getUniqueChunk(true, ORIENTATION_TAG, DataAttribute(), shapeChunk);
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

        auto radiusChunk = dataParser.getUniqueChunk(true, RADIUS_TAG, DataAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto heightChunk = dataParser.getUniqueChunk(true, HEIGHT_TAG, DataAttribute(), shapeChunk);
        float height = heightChunk->getFloatValue();

        return new CollisionConeShape(radius, height, orientation);
    }

    void CollisionConeReaderWriter::writeOn(XmlChunk* shapeChunk, const CollisionShape3D& collisionShape, XmlWriter& xmlWriter) const {
        shapeChunk->setAttribute(DataAttribute(TYPE_ATTR, CONE_VALUE));

        const auto& coneShape = dynamic_cast<const CollisionConeShape&>(collisionShape);

        auto orientationChunk = xmlWriter.createChunk(ORIENTATION_TAG, DataAttribute(), shapeChunk);
        ConeShape<float>::ConeOrientation orientationValue = coneShape.getConeOrientation();
        if (orientationValue == ConeShape<float>::ConeOrientation::CONE_X_POSITIVE) {
            orientationChunk->setStringValue(X_POSITIVE_VALUE);
        } else if (orientationValue == ConeShape<float>::ConeOrientation::CONE_X_NEGATIVE) {
            orientationChunk->setStringValue(X_NEGATIVE_VALUE);
        } else if (orientationValue == ConeShape<float>::ConeOrientation::CONE_Y_POSITIVE) {
            orientationChunk->setStringValue(Y_POSITIVE_VALUE);
        } else if (orientationValue == ConeShape<float>::ConeOrientation::CONE_Y_NEGATIVE) {
            orientationChunk->setStringValue(Y_NEGATIVE_VALUE);
        } else if (orientationValue == ConeShape<float>::ConeOrientation::CONE_Z_POSITIVE) {
            orientationChunk->setStringValue(Z_POSITIVE_VALUE);
        } else if (orientationValue == ConeShape<float>::ConeOrientation::CONE_Z_NEGATIVE) {
            orientationChunk->setStringValue(Z_NEGATIVE_VALUE);
        } else {
            throw std::invalid_argument("Cone orientation type unknown: " + std::to_string(orientationValue));
        }

        auto radiusChunk = xmlWriter.createChunk(RADIUS_TAG, DataAttribute(), shapeChunk);
        radiusChunk->setFloatValue(coneShape.getRadius());

        auto heightChunk = xmlWriter.createChunk(HEIGHT_TAG, DataAttribute(), shapeChunk);
        heightChunk->setFloatValue(coneShape.getHeight());
    }

}
