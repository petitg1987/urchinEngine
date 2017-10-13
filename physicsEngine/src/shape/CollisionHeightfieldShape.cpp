#include <limits>
#include <cmath>
#include <cassert>
#include <utility>

#include "CollisionHeightfieldShape.h"

namespace urchin
{

    CollisionHeightfieldShape::CollisionHeightfieldShape(std::vector<Point3<float>> vertices, unsigned int width, unsigned int height) :
            CollisionShape3D(),
            vertices(std::move(vertices)),
            width(width),
            height(height)
    {
        assert(vertices.size()==width*height);
        localAABBox = buildLocalAABBox();

        lastTransform.setPosition(Point3<float>(NAN, NAN, NAN));
    }

    std::unique_ptr<BoxShape<float>> CollisionHeightfieldShape::buildLocalAABBox() const
    {
        //TODO ...
    }

    CollisionShape3D::ShapeType CollisionHeightfieldShape::getShapeType() const
    {
        return CollisionHeightfieldShape::TRIANGLE_MESH_SHAPE;
    }

    std::shared_ptr<ConvexShape3D<float>> CollisionHeightfieldShape::getSingleShape() const
    {
        throw std::runtime_error("Impossible to retrieve single convex shape for heightfield shape");
    }

    std::shared_ptr<CollisionShape3D> CollisionHeightfieldShape::scale(float scale) const
    {
        if((scale - std::numeric_limits<float>::min()) > 1.0 || (scale + std::numeric_limits<float>::min()) < 0.0)
        {
            throw std::runtime_error("Scaling a heightfield shape is currently not supported");
        }

        return std::make_shared<CollisionHeightfieldShape>(vertices, width, height);
    }

    AABBox<float> CollisionHeightfieldShape::toAABBox(const PhysicsTransform &physicsTransform) const
    {
        if(!lastTransform.equals(physicsTransform))
        {
            const Matrix3<float> &orientation = physicsTransform.retrieveOrientationMatrix();
            Point3<float> extend(
                    localAABBox->getHalfSize(0) * std::abs(orientation[0]) + localAABBox->getHalfSize(1) * std::abs(orientation[3]) + localAABBox->getHalfSize(2) * std::abs(orientation[6]),
                    localAABBox->getHalfSize(0) * std::abs(orientation[1]) + localAABBox->getHalfSize(1) * std::abs(orientation[4]) + localAABBox->getHalfSize(2) * std::abs(orientation[7]),
                    localAABBox->getHalfSize(0) * std::abs(orientation[2]) + localAABBox->getHalfSize(1) * std::abs(orientation[5]) + localAABBox->getHalfSize(2) * std::abs(orientation[8])
            );

            const Point3<float> &position = physicsTransform.getPosition();

            lastTransform = physicsTransform;
        }

        return lastAABBox;
    }

    std::shared_ptr<CollisionConvexObject3D> CollisionHeightfieldShape::toConvexObject(const PhysicsTransform &physicsTransform) const
    {
        throw std::runtime_error("Impossible to transform heightfield shape to convex object");
    }

    Vector3<float> CollisionHeightfieldShape::computeLocalInertia(float mass) const
    {
        AABBox<float> aabbox = toAABBox(PhysicsTransform());
        float width = 2.0f * aabbox.getHalfSize(0);
        float height = 2.0f * aabbox.getHalfSize(1);
        float depth = 2.0f * aabbox.getHalfSize(2);

        float localInertia1 = (1.0f/12.0f) * mass * (height*height + depth*depth);
        float localInertia2 = (1.0f/12.0f) * mass * (width*width + depth*depth);
        float localInertia3 = (1.0f/12.0f) * mass * (width*width + height*height);
        return Vector3<float>(localInertia1, localInertia2, localInertia3);
    }

    float CollisionHeightfieldShape::getMaxDistanceToCenter() const
    {
        throw std::runtime_error("Impossible to get max distance to center for heightfield shape. Only static bodies are supported for this shape.");
    }

    float CollisionHeightfieldShape::getMinDistanceToCenter() const
    {
        return 0.0f;
    }
}
