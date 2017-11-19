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
            auto singleShape = std::make_shared<AIShape>(scaledShape->getSingleShape());
            return std::make_shared<AIObject>(name, singleShape, unscaledTransform, true);
        }else if(scaledShape->isCompound())
        {
            std::vector<std::shared_ptr<AIShape>> aiShapes;
            std::shared_ptr<const CollisionCompoundShape> scaledCompoundShape = std::dynamic_pointer_cast<const CollisionCompoundShape>(scaledShape);
            for(const auto &scaledLocalizedShape : scaledCompoundShape->getLocalizedShapes())
            {
                aiShapes.push_back(std::make_shared<AIShape>(scaledLocalizedShape->shape->getSingleShape(), scaledLocalizedShape->transform.toTransform()));
            }

            return std::make_shared<AIObject>(name, true, aiShapes, unscaledTransform, true);
        }else if(scaledShape->isConcave())
        {
            std::shared_ptr<const CollisionHeightfieldShape> scaledHeightfieldShape = std::dynamic_pointer_cast<const CollisionHeightfieldShape>(scaledShape);
            if(scaledHeightfieldShape!=nullptr)
            {
                return heightfieldToAIObject(name, scaledHeightfieldShape, unscaledTransform);
            }else
            {
                throw std::invalid_argument("Unknown concave shape type: " + std::string(typeid(*scaledShape).name()));
            }
        }

        throw std::invalid_argument("Unknown shape type category: " + std::to_string(scaledShape->getShapeType()));
    }

    std::shared_ptr<AIObject> AIObjectBuilder::heightfieldToAIObject(std::string name, const std::shared_ptr<const CollisionHeightfieldShape> &heightfieldShape, const Transform<float> &unscaledTransform)
    { //TODO handle division when exist big difference of level
        std::vector<std::shared_ptr<AIShape>> aiShapes;

        unsigned int subDivision = 15;
        float xDivisionLength = heightfieldShape->getXLength() / static_cast<float>(subDivision);
        float zDivisionLength = heightfieldShape->getZLength() / static_cast<float>(subDivision);

        for(unsigned int x=0; x<subDivision; x++)
        {
            auto startX = static_cast<unsigned int>(((float)x) * xDivisionLength);
            auto endX = static_cast<unsigned int>(((float)x + 1.0f) * xDivisionLength) - 1;

            for(unsigned int z=0; z<subDivision; ++z)
            {
                auto startZ = static_cast<unsigned int>(((float)z) * zDivisionLength);
                auto endZ = static_cast<unsigned int>(((float)z + 1.0f) * zDivisionLength) - 1;

                std::vector<Point3<float>> squareVertices = getHeightfieldVertices(heightfieldShape, startX, endX, startZ, endZ);

                unsigned int squareXLength = endX-startX;
                unsigned int squareZLength = endZ-startZ;

                Point3<float> p1 = squareVertices[0];
                Point3<float> p2 = squareVertices[squareXLength - 1];
                Point3<float> p3 = squareVertices[(squareXLength * squareZLength) - 1];
                Point3<float> p4 = squareVertices[(squareXLength * squareZLength) - squareXLength];

                auto triangleShape1 = std::make_shared<TriangleShape3D<float>>(p1, p2, p3);
                auto shape1 = std::make_shared<AIShape>(triangleShape1);
                aiShapes.push_back(shape1);

                auto triangleShape2 = std::make_shared<TriangleShape3D<float>>(p1, p3, p4);
                auto shape2 = std::make_shared<AIShape>(triangleShape2);
                aiShapes.push_back(shape2);
            }
        }

        return std::make_shared<AIObject>(name, false, aiShapes, unscaledTransform, false);
    }

    std::vector<Point3<float>> AIObjectBuilder::getHeightfieldVertices(const std::shared_ptr<const CollisionHeightfieldShape> &heightfieldShape,
                                                 unsigned int startX, unsigned int endX, unsigned int startZ, unsigned int endZ)
    {
        std::vector<Point3<float>> squareVertices;
        squareVertices.reserve((endZ-startZ) * (endX-startX));

        for(unsigned int z=startZ; z<endZ; ++z)
        {
            for(unsigned int x=startX; x<endX; ++x)
            {
                squareVertices.push_back(heightfieldShape->getVertices()[z * heightfieldShape->getXLength() + x]);
            }
        }

        return squareVertices;
    }


}
