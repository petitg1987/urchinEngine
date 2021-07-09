#include <stdexcept>

#include "CollisionCylinderReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionCylinderReaderWriter::loadFrom(const XmlChunk* shapeChunk, const XmlParser& xmlParser) const {
        auto orientationChunk = xmlParser.getUniqueChunk(true, ORIENTATION_TAG, DataAttribute(), shapeChunk);
        std::string orientationValue = orientationChunk->getStringValue();
        CylinderShape<float>::CylinderOrientation orientation;
        if (orientationValue == X_VALUE) {
            orientation = CylinderShape<float>::CylinderOrientation::CYLINDER_X;
        } else if (orientationValue == Y_VALUE) {
            orientation = CylinderShape<float>::CylinderOrientation::CYLINDER_Y;
        } else if (orientationValue == Z_VALUE) {
            orientation = CylinderShape<float>::CylinderOrientation::CYLINDER_Z;
        } else {
            throw std::invalid_argument("Cylinder orientation type unknown: " + orientationValue);
        }

        auto radiusChunk = xmlParser.getUniqueChunk(true, RADIUS_TAG, DataAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto heightChunk = xmlParser.getUniqueChunk(true, HEIGHT_TAG, DataAttribute(), shapeChunk);
        float height = heightChunk->getFloatValue();

        return new CollisionCylinderShape(radius, height, orientation);
    }

    void CollisionCylinderReaderWriter::writeOn(XmlChunk* shapeChunk, const CollisionShape3D& collisionShape, XmlWriter& xmlWriter) const {
        shapeChunk->setAttribute(DataAttribute(TYPE_ATTR, CYLINDER_VALUE));

        const auto& cylinderShape = dynamic_cast<const CollisionCylinderShape&>(collisionShape);

        auto orientationChunk = xmlWriter.createChunk(ORIENTATION_TAG, DataAttribute(), shapeChunk);
        CylinderShape<float>::CylinderOrientation orientationValue = cylinderShape.getCylinderOrientation();
        if (orientationValue == CylinderShape<float>::CylinderOrientation::CYLINDER_X) {
            orientationChunk->setStringValue(X_VALUE);
        } else if (orientationValue == CylinderShape<float>::CylinderOrientation::CYLINDER_Y) {
            orientationChunk->setStringValue(Y_VALUE);
        } else if (orientationValue == CylinderShape<float>::CylinderOrientation::CYLINDER_Z) {
            orientationChunk->setStringValue(Z_VALUE);
        } else {
            throw std::invalid_argument("Cylinder orientation type unknown: " + std::to_string(orientationValue));
        }

        auto radiusChunk = xmlWriter.createChunk(RADIUS_TAG, DataAttribute(), shapeChunk);
        radiusChunk->setFloatValue(cylinderShape.getRadius());

        auto heightChunk = xmlWriter.createChunk(HEIGHT_TAG, DataAttribute(), shapeChunk);
        heightChunk->setFloatValue(cylinderShape.getHeight());
    }

}
