#include <stdexcept>
#include <vector>

#include <resources/common/AIEntityBuilder.h>

namespace urchin {
    std::shared_ptr<AIObject> AIEntityBuilder::buildAIObject(const std::string& name, const CollisionShape3D& shape, const Transform<float>& transform) {
        assert(MathFunction::isOne(transform.getScale(), 0.01f));

        if (shape.isConvex()) {
            auto singleShape = std::make_unique<AIShape>(shape.getSingleShape().clone());
            return std::make_shared<AIObject>(name, transform, true, std::move(singleShape));
        } else if (shape.isCompound()) {
            std::vector<std::unique_ptr<AIShape>> aiShapes;
            const auto& scaledCompoundShape = dynamic_cast<const CollisionCompoundShape&>(shape);
            for (const auto& scaledLocalizedShape : scaledCompoundShape.getLocalizedShapes()) {
                aiShapes.push_back(std::make_unique<AIShape>(scaledLocalizedShape->shape->getSingleShape().clone(), scaledLocalizedShape->transform.toTransform()));
            }

            return std::make_shared<AIObject>(name, transform, true, std::move(aiShapes));
        }

        throw std::invalid_argument("Unknown shape type category: " + std::to_string(shape.getShapeType()));
    }

    std::shared_ptr<AITerrain> AIEntityBuilder::buildAITerrain(const std::string& name, const CollisionShape3D& shape, const Transform<float>& transform) {
        assert(MathFunction::isOne(transform.getScale(), 0.01f));

        try{
            const auto& scaledHeightfieldShape = dynamic_cast<const CollisionHeightfieldShape&>(shape);
            return buildAITerrain(name, scaledHeightfieldShape, transform);
        } catch(const std::bad_cast& e) {
            throw std::invalid_argument("Unknown terrain shape type: " + std::string(typeid(shape).name()));
        }
    }

    std::shared_ptr<AITerrain> AIEntityBuilder::buildAITerrain(const std::string& name, const CollisionHeightfieldShape& heightfieldShape, const Transform<float>& transform) {
        return std::make_shared<AITerrain>(name, transform, false, heightfieldShape.getVertices(), heightfieldShape.getXLength(), heightfieldShape.getZLength());
    }
}
