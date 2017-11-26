#include <stdexcept>
#include <vector>

#include "AIEntityBuilder.h"

namespace urchin
{
    std::shared_ptr<AIObject> AIEntityBuilder::buildAIObject(std::string name, const std::shared_ptr<const CollisionShape3D> &scaledShape, const Transform<float> &transform)
    {
        Transform<float> unscaledTransform = transform;
        unscaledTransform.setScale(1.0); //scale not needed because shape is already scaled.

        if(scaledShape->isConvex())
        {
            auto singleShape = std::make_shared<AIShape>(scaledShape->getSingleShape());
            return std::make_shared<AIObject>(name, unscaledTransform, true, singleShape);
        }else if(scaledShape->isCompound())
        {
            std::vector<std::shared_ptr<AIShape>> aiShapes;
            std::shared_ptr<const CollisionCompoundShape> scaledCompoundShape = std::dynamic_pointer_cast<const CollisionCompoundShape>(scaledShape);
            for(const auto &scaledLocalizedShape : scaledCompoundShape->getLocalizedShapes())
            {
                aiShapes.push_back(std::make_shared<AIShape>(scaledLocalizedShape->shape->getSingleShape(), scaledLocalizedShape->transform.toTransform()));
            }

            return std::make_shared<AIObject>(name, unscaledTransform, true, aiShapes);
        }

        throw std::invalid_argument("Unknown shape type category: " + std::to_string(scaledShape->getShapeType()));
    }

    std::shared_ptr<AITerrain> AIEntityBuilder::buildAITerrain(std::string name, const std::shared_ptr<const CollisionShape3D> &scaledShape, const Transform<float> &transform)
    {
        Transform<float> unscaledTransform = transform;
        unscaledTransform.setScale(1.0); //scale not needed because shape is already scaled.

        std::shared_ptr<const CollisionHeightfieldShape> scaledHeightfieldShape = std::dynamic_pointer_cast<const CollisionHeightfieldShape>(scaledShape);
        if(scaledHeightfieldShape!=nullptr)
        {
            return buildAITerrain(std::move(name), scaledHeightfieldShape, unscaledTransform);
        }else
        {
            throw std::invalid_argument("Unknown terrain shape type: " + std::string(typeid(*scaledShape).name()));
        }
    }

    std::shared_ptr<AITerrain> AIEntityBuilder::buildAITerrain(std::string name, const std::shared_ptr<const CollisionHeightfieldShape> &heightfieldShape, const Transform<float> &transform)
    {
        //TODO use heightfieldShape to build AITerrain
        return std::make_shared<AITerrain>(name, transform, false);
    }
}
