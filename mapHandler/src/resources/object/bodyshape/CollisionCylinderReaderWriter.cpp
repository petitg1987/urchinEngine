#include <stdexcept>

#include "CollisionCylinderReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionCylinderReaderWriter::loadFrom(const DataChunk* shapeChunk, const DataParser& dataParser) const {
        auto orientationChunk = dataParser.getUniqueChunk(true, ORIENTATION_TAG, UdaAttribute(), shapeChunk);
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

        auto radiusChunk = dataParser.getUniqueChunk(true, RADIUS_TAG, UdaAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto heightChunk = dataParser.getUniqueChunk(true, HEIGHT_TAG, UdaAttribute(), shapeChunk);
        float height = heightChunk->getFloatValue();

        return new CollisionCylinderShape(radius, height, orientation);
    }

    void CollisionCylinderReaderWriter::writeOn(DataChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaWriter& udaWriter) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, CYLINDER_VALUE));

        const auto& cylinderShape = dynamic_cast<const CollisionCylinderShape&>(collisionShape);

        auto& orientationChunk = udaWriter.createChunk(ORIENTATION_TAG, UdaAttribute(), &shapeChunk);
        CylinderShape<float>::CylinderOrientation orientationValue = cylinderShape.getCylinderOrientation();
        if (orientationValue == CylinderShape<float>::CylinderOrientation::CYLINDER_X) {
            orientationChunk.setStringValue(X_VALUE);
        } else if (orientationValue == CylinderShape<float>::CylinderOrientation::CYLINDER_Y) {
            orientationChunk.setStringValue(Y_VALUE);
        } else if (orientationValue == CylinderShape<float>::CylinderOrientation::CYLINDER_Z) {
            orientationChunk.setStringValue(Z_VALUE);
        } else {
            throw std::invalid_argument("Cylinder orientation type unknown: " + std::to_string(orientationValue));
        }

        auto& radiusChunk = udaWriter.createChunk(RADIUS_TAG, UdaAttribute(), &shapeChunk);
        radiusChunk.setFloatValue(cylinderShape.getRadius());

        auto& heightChunk = udaWriter.createChunk(HEIGHT_TAG, UdaAttribute(), &shapeChunk);
        heightChunk.setFloatValue(cylinderShape.getHeight());
    }

}
