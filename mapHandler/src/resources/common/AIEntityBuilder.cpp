#include <stdexcept>
#include <vector>

#include "AIEntityBuilder.h"

namespace urchin {
    std::shared_ptr<AIObject> AIEntityBuilder::buildAIObject(const std::string& name, const std::shared_ptr<const CollisionShape3D>& shape, const Transform<float>& transform) {
        assert(MathFunction::isOne(transform.getScale(), 0.01f));

        if (shape->isConvex()) {
            auto singleShape = std::make_shared<AIShape>(shape->getSingleShape());
            return std::make_shared<AIObject>(name, transform, true, singleShape);
        } else if (shape->isCompound()) {
            std::vector<std::shared_ptr<AIShape>> aiShapes;
            std::shared_ptr<const CollisionCompoundShape> scaledCompoundShape = std::dynamic_pointer_cast<const CollisionCompoundShape>(shape);
            for (const auto& scaledLocalizedShape : scaledCompoundShape->getLocalizedShapes()) {
                aiShapes.push_back(std::make_shared<AIShape>(scaledLocalizedShape->shape->getSingleShape(), scaledLocalizedShape->transform.toTransform()));
            }

            return std::make_shared<AIObject>(name, transform, true, aiShapes);
        }

        throw std::invalid_argument("Unknown shape type category: " + std::to_string(shape->getShapeType()));
    }

    std::shared_ptr<AITerrain> AIEntityBuilder::buildAITerrain(const std::string& name, const std::shared_ptr<const CollisionShape3D>& shape, const Transform<float>& transform) {
        assert(MathFunction::isOne(transform.getScale(), 0.01f));

        std::shared_ptr<const CollisionHeightfieldShape> scaledHeightfieldShape = std::dynamic_pointer_cast<const CollisionHeightfieldShape>(shape);
        if (scaledHeightfieldShape) {
            return buildAITerrain(name, scaledHeightfieldShape, transform);
        } else {
            throw std::invalid_argument("Unknown terrain shape type: " + std::string(typeid(*shape).name()));
        }
    }

    std::shared_ptr<AITerrain> AIEntityBuilder::buildAITerrain(const std::string& name, const std::shared_ptr<const CollisionHeightfieldShape>& heightfieldShape, const Transform<float>& transform) {
        return std::make_shared<AITerrain>(name, transform, false, heightfieldShape->getVertices(), heightfieldShape->getXLength(), heightfieldShape->getZLength());
    }
}
