#include <stdexcept>

#include "CollisionConeReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionConeReaderWriter::loadFrom(const std::shared_ptr<XmlChunk>& shapeChunk, const XmlParser& xmlParser) const {
        std::shared_ptr<XmlChunk> orientationChunk = xmlParser.getUniqueChunk(true, ORIENTATION_TAG, XmlAttribute(), shapeChunk);
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

        std::shared_ptr<XmlChunk> radiusChunk = xmlParser.getUniqueChunk(true, RADIUS_TAG, XmlAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        std::shared_ptr<XmlChunk> heightChunk = xmlParser.getUniqueChunk(true, HEIGHT_TAG, XmlAttribute(), shapeChunk);
        float height = heightChunk->getFloatValue();

        return new CollisionConeShape(radius, height, orientation);
    }

    void CollisionConeReaderWriter::writeOn(const std::shared_ptr<XmlChunk>& shapeChunk, const CollisionShape3D* collisionShape, XmlWriter& xmlWriter) const {
        shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, CONE_VALUE));

        const auto* coneShape = dynamic_cast<const CollisionConeShape*>(collisionShape);

        std::shared_ptr<XmlChunk> orientationChunk = xmlWriter.createChunk(ORIENTATION_TAG, XmlAttribute(), shapeChunk);
        ConeShape<float>::ConeOrientation orientationValue = coneShape->getConeOrientation();
        if (orientationValue==ConeShape<float>::ConeOrientation::CONE_X_POSITIVE) {
            orientationChunk->setStringValue(X_POSITIVE_VALUE);
        } else if (orientationValue==ConeShape<float>::ConeOrientation::CONE_X_NEGATIVE) {
            orientationChunk->setStringValue(X_NEGATIVE_VALUE);
        } else if (orientationValue==ConeShape<float>::ConeOrientation::CONE_Y_POSITIVE) {
            orientationChunk->setStringValue(Y_POSITIVE_VALUE);
        } else if (orientationValue==ConeShape<float>::ConeOrientation::CONE_Y_NEGATIVE) {
            orientationChunk->setStringValue(Y_NEGATIVE_VALUE);
        } else if (orientationValue==ConeShape<float>::ConeOrientation::CONE_Z_POSITIVE) {
            orientationChunk->setStringValue(Z_POSITIVE_VALUE);
        } else if (orientationValue==ConeShape<float>::ConeOrientation::CONE_Z_NEGATIVE) {
            orientationChunk->setStringValue(Z_NEGATIVE_VALUE);
        } else {
            throw std::invalid_argument("Cone orientation type unknown: " + std::to_string(orientationValue));
        }

        std::shared_ptr<XmlChunk> radiusChunk = xmlWriter.createChunk(RADIUS_TAG, XmlAttribute(), shapeChunk);
        radiusChunk->setFloatValue(coneShape->getRadius());

        std::shared_ptr<XmlChunk> heightChunk = xmlWriter.createChunk(HEIGHT_TAG, XmlAttribute(), shapeChunk);
        heightChunk->setFloatValue(coneShape->getHeight());
    }

}
