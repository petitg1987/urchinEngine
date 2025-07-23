#include <stdexcept>

#include "map/save/object/physics/bodyshape/CollisionCylinderReaderWriter.h"

namespace urchin {

    std::unique_ptr<CollisionShape3D> CollisionCylinderReaderWriter::load(const UdaChunk* shapeChunk, const UdaParser& udaParser) const {
        auto orientationChunk = udaParser.getFirstChunk(true, ORIENTATION_TAG, UdaAttribute(), shapeChunk);
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

        auto radiusChunk = udaParser.getFirstChunk(true, RADIUS_TAG, UdaAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto heightChunk = udaParser.getFirstChunk(true, HEIGHT_TAG, UdaAttribute(), shapeChunk);
        float height = heightChunk->getFloatValue();

        return std::make_unique<CollisionCylinderShape>(radius, height, orientation);
    }

    void CollisionCylinderReaderWriter::write(UdaChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaParser& udaParser) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, CYLINDER_VALUE));

        const auto& cylinderShape = static_cast<const CollisionCylinderShape&>(collisionShape);

        auto& orientationChunk = udaParser.createChunk(ORIENTATION_TAG, UdaAttribute(), &shapeChunk);
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

        auto& radiusChunk = udaParser.createChunk(RADIUS_TAG, UdaAttribute(), &shapeChunk);
        radiusChunk.setFloatValue(cylinderShape.getRadius());

        auto& heightChunk = udaParser.createChunk(HEIGHT_TAG, UdaAttribute(), &shapeChunk);
        heightChunk.setFloatValue(cylinderShape.getHeight());
    }

}
