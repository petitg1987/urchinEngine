#include <stdexcept>

#include "CollisionCylinderReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionCylinderReaderWriter::loadFrom(const std::shared_ptr<XmlChunk>& shapeChunk, const XmlParser& xmlParser) const {
        std::shared_ptr<XmlChunk> orientationChunk = xmlParser.getUniqueChunk(true, ORIENTATION_TAG, XmlAttribute(), shapeChunk);
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

        std::shared_ptr<XmlChunk> radiusChunk = xmlParser.getUniqueChunk(true, RADIUS_TAG, XmlAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        std::shared_ptr<XmlChunk> heightChunk = xmlParser.getUniqueChunk(true, HEIGHT_TAG, XmlAttribute(), shapeChunk);
        float height = heightChunk->getFloatValue();

        return new CollisionCylinderShape(radius, height, orientation);
    }

    void CollisionCylinderReaderWriter::writeOn(const std::shared_ptr<XmlChunk>& shapeChunk, const CollisionShape3D* collisionShape, XmlWriter& xmlWriter) const {
        shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, CYLINDER_VALUE));

        const auto* cylinderShape = dynamic_cast<const CollisionCylinderShape*>(collisionShape);

        std::shared_ptr<XmlChunk> orientationChunk = xmlWriter.createChunk(ORIENTATION_TAG, XmlAttribute(), shapeChunk);
        CylinderShape<float>::CylinderOrientation orientationValue = cylinderShape->getCylinderOrientation();
        if (orientationValue == CylinderShape<float>::CylinderOrientation::CYLINDER_X) {
            orientationChunk->setStringValue(X_VALUE);
        } else if (orientationValue == CylinderShape<float>::CylinderOrientation::CYLINDER_Y) {
            orientationChunk->setStringValue(Y_VALUE);
        } else if (orientationValue == CylinderShape<float>::CylinderOrientation::CYLINDER_Z) {
            orientationChunk->setStringValue(Z_VALUE);
        } else {
            throw std::invalid_argument("Cylinder orientation type unknown: " + std::to_string(orientationValue));
        }

        std::shared_ptr<XmlChunk> radiusChunk = xmlWriter.createChunk(RADIUS_TAG, XmlAttribute(), shapeChunk);
        radiusChunk->setFloatValue(cylinderShape->getRadius());

        std::shared_ptr<XmlChunk> heightChunk = xmlWriter.createChunk(HEIGHT_TAG, XmlAttribute(), shapeChunk);
        heightChunk->setFloatValue(cylinderShape->getHeight());
    }

}
