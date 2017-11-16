#include <stdexcept>
#include <vector>

#include "AIObjectBuilder.h"

namespace urchin
{
    std::shared_ptr<AIObject> AIObjectBuilder::buildAIObject(std::string name, const std::shared_ptr<const CollisionShape3D> &scaledShape, const Transform<float> &transform)
    {
        Transform<float> unscaledTransform = transform;
        unscaledTransform.setScale(1.0); //scale not needed because shape is already scaled.

        if(scaledShape->isConvex())
        {
            return std::make_shared<AIObject>(name, std::make_shared<AIShape>(scaledShape->getSingleShape()), unscaledTransform);
        }else if(scaledShape->isCompound())
        {
            std::vector<std::shared_ptr<AIShape>> aiShapes;
            std::shared_ptr<const CollisionCompoundShape> scaledCompoundShape = std::dynamic_pointer_cast<const CollisionCompoundShape>(scaledShape);
            for(const auto &scaledLocalizedShape : scaledCompoundShape->getLocalizedShapes())
            {
                aiShapes.push_back(std::make_shared<AIShape>(scaledLocalizedShape->shape->getSingleShape(), scaledLocalizedShape->transform.toTransform()));
            }

            return std::make_shared<AIObject>(name, aiShapes, unscaledTransform);
        }else if(scaledShape->isConcave())
        {
            //TODO build terrain shapes
            std::vector<std::shared_ptr<AIShape>> aiShapes;
            return std::make_shared<AIObject>(name, aiShapes, unscaledTransform);
        }

        throw std::invalid_argument("Unknown shape type category: " + std::to_string(scaledShape->getShapeType()));
    }


}
