#include <stdexcept>
#include <vector>

#include "model/builder/AIEntityBuilder.h"

namespace urchin {

    std::shared_ptr<AIObject> AIEntityBuilder::buildAIObject(std::string name, const CollisionShape3D& shape, const Transform<float>& transform) {
        assert(!transform.hasScaling());

        if (shape.isConvex()) {
            auto singleShape = std::make_unique<AIShape>(shape.getSingleShape().clone());
            return std::make_shared<AIObject>(std::move(name), transform, true, std::move(singleShape));
        } else if (shape.isCompound()) {
            const auto& scaledCompoundShape = static_cast<const CollisionCompoundShape&>(shape);
            std::vector<std::unique_ptr<AIShape>> aiShapes;
            aiShapes.reserve(scaledCompoundShape.getLocalizedShapes().size());
            for (const auto& scaledLocalizedShape : scaledCompoundShape.getLocalizedShapes()) {
                aiShapes.push_back(std::make_unique<AIShape>(scaledLocalizedShape->shape->getSingleShape().clone(), scaledLocalizedShape->transform.toTransform()));
            }

            return std::make_shared<AIObject>(std::move(name), transform, true, std::move(aiShapes));
        }

        throw std::invalid_argument("Unknown shape type category: " + std::to_string(shape.getShapeType()));
    }

    std::shared_ptr<AITerrain> AIEntityBuilder::buildAITerrain(std::string name, const CollisionShape3D& shape, const Transform<float>& transform) {
        assert(!transform.hasScaling());

        if (shape.getShapeType() == CollisionShape3D::ShapeType::HEIGHTFIELD_SHAPE) {
            const auto& scaledHeightfieldShape = static_cast<const CollisionHeightfieldShape&>(shape);
            return std::make_shared<AITerrain>(std::move(name), transform, false, scaledHeightfieldShape.getVertices(), scaledHeightfieldShape.getXLength(), scaledHeightfieldShape.getZLength());
        } else {
            throw std::invalid_argument("Unknown terrain shape type: " + std::string(typeid(shape).name()));
        }
    }

}
