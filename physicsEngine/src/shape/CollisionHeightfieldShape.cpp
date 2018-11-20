#include <limits>
#include <cmath>
#include <cassert>
#include <utility>

#include "CollisionHeightfieldShape.h"

namespace urchin
{

    CollisionHeightfieldShape::CollisionHeightfieldShape(std::vector<Point3<float>> vertices, unsigned int xLength, unsigned int zLength) :
            CollisionShape3D(),
            vertices(std::move(vertices)),
            xLength(xLength),
            zLength(zLength)
    {
        lastTransform.setPosition(Point3<float>(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));

        assert(this->vertices.size()==xLength*zLength);
        localAABBox = buildLocalAABBox();

        unsigned int trianglesPoolSize = ConfigService::instance()->getUnsignedIntValue("collisionShape.heightfieldTriangleShapesPoolSize");
        triangleShapesPool = new FixedSizePool<TriangleShape3D<float>>("triangleShapesPool", sizeof(TriangleShape3D<float>), trianglesPoolSize);
    }

    CollisionHeightfieldShape::~CollisionHeightfieldShape()
    {
        trianglesInAABBox.clear();
        delete triangleShapesPool;
    }

    std::unique_ptr<BoxShape<float>> CollisionHeightfieldShape::buildLocalAABBox() const
    {
        Point3<float> min(vertices[0].X, std::numeric_limits<float>::max(), vertices[0].Z);
        Point3<float> max(vertices[xLength-1].X, -std::numeric_limits<float>::max(), vertices[vertices.size()-1].Z);

        for (const auto &vertex : vertices)
        {
            if(min.Y > vertex.Y)
            {
                min.Y = vertex.Y;
            }
            if(max.Y < vertex.Y)
            {
                max.Y = vertex.Y;
            }
        }

        //center on Y axis
        float maxAbsoluteYValue = std::max(std::abs(min.Y), std::abs(max.Y));
        min.Y = -maxAbsoluteYValue;
        max.Y = maxAbsoluteYValue;

        #ifdef _DEBUG //check BoxShape is centered
            assert(std::abs(std::abs(min.X)-max.X) < 0.01);
            assert(std::abs(std::abs(min.Y)-max.Y) < 0.01);
            assert(std::abs(std::abs(min.Z)-max.Z) < 0.01);
        #endif

        Vector3<float> halfSizes((max.X-min.X) / 2.0, (max.Y-min.Y) / 2.0, (max.Z-min.Z) / 2.0);
        return std::make_unique<BoxShape<float>>(halfSizes);
    }

    CollisionShape3D::ShapeType CollisionHeightfieldShape::getShapeType() const
    {
        return CollisionHeightfieldShape::HEIGHTFIELD_SHAPE;
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

        return std::make_shared<CollisionHeightfieldShape>(vertices, xLength, zLength);
    }

    const std::vector<Point3<float>> &CollisionHeightfieldShape::getVertices() const
    {
        return vertices;
    }

    unsigned int CollisionHeightfieldShape::getXLength() const
    {
        return xLength;
    }

    unsigned int CollisionHeightfieldShape::getZLength() const
    {
        return zLength;
    }

    AABBox<float> CollisionHeightfieldShape::toAABBox(const PhysicsTransform &physicsTransform) const
    {
        if(!lastTransform.equals(physicsTransform))
        {
            const Matrix3<float> &orientation = physicsTransform.retrieveOrientationMatrix();
            Point3<float> extend(
                    localAABBox->getHalfSize(0) * std::abs(orientation(0)) + localAABBox->getHalfSize(1) * std::abs(orientation(3)) + localAABBox->getHalfSize(2) * std::abs(orientation(6)),
                    localAABBox->getHalfSize(0) * std::abs(orientation(1)) + localAABBox->getHalfSize(1) * std::abs(orientation(4)) + localAABBox->getHalfSize(2) * std::abs(orientation(7)),
                    localAABBox->getHalfSize(0) * std::abs(orientation(2)) + localAABBox->getHalfSize(1) * std::abs(orientation(5)) + localAABBox->getHalfSize(2) * std::abs(orientation(8))
            );

            const Point3<float> &position = physicsTransform.getPosition();

            lastAABBox = AABBox<float>(position - extend, position + extend);
            lastTransform = physicsTransform;
        }

        return lastAABBox;
    }

    CollisionConvexObject3D *CollisionHeightfieldShape::toConvexObject(const PhysicsTransform &physicsTransform) const
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
        throw std::runtime_error("Impossible to get max distance to center for heightfield shape. A heightfield body must be static.");
    }

    float CollisionHeightfieldShape::getMinDistanceToCenter() const
    {
        return 0.0f;
    }

    CollisionShape3D *CollisionHeightfieldShape::clone() const
    {
        return new CollisionHeightfieldShape(vertices, xLength, zLength);
    }

    const std::vector<CollisionTriangleShape> &CollisionHeightfieldShape::findTrianglesInAABBox(const AABBox<float> &checkAABBox) const
    {
        trianglesInAABBox.clear();

        float verticesDistanceX = vertices[1].X - vertices[0].X;
        auto rawStartVertexX = static_cast<int>((checkAABBox.getMin().X + localAABBox->getHalfSizes().X) / verticesDistanceX);
        auto startVertexX = static_cast<unsigned int>(MathAlgorithm::clamp(rawStartVertexX, 0, static_cast<int>(xLength-1)));
        auto rawEndVertexX = static_cast<int>((checkAABBox.getMax().X + localAABBox->getHalfSizes().X) / verticesDistanceX) + 1;
        auto endVertexX = static_cast<unsigned int>(MathAlgorithm::clamp(rawEndVertexX, 0, static_cast<int>(xLength-1)));

        float verticesDistanceZ = vertices[xLength].Z - vertices[0].Z;
        auto rawStartVertexZ = static_cast<int>((checkAABBox.getMin().Z + localAABBox->getHalfSizes().Z) / verticesDistanceZ);
        auto startVertexZ = static_cast<unsigned int>(MathAlgorithm::clamp(rawStartVertexZ, 0, static_cast<int>(zLength-1)));
        auto rawEndVertexZ = static_cast<int>((checkAABBox.getMax().Z + localAABBox->getHalfSizes().Z) / verticesDistanceZ) + 1;
        auto endVertexZ = static_cast<unsigned int>(MathAlgorithm::clamp(rawEndVertexZ, 0, static_cast<int>(zLength-1)));

        for(unsigned int z = startVertexZ; z < endVertexZ; ++z)
        {
            for (unsigned int x = startVertexX; x < endVertexX; ++x)
            {
                Point3<float> point1 = vertices[x + xLength * z]; //far-left
                Point3<float> point2 = vertices[x + 1 + xLength * z]; //far-right
                Point3<float> point3 = vertices[x + xLength * (z + 1)]; //near-left
                Point3<float> point4 = vertices[x + 1 + xLength * (z + 1)]; //near-right

                bool hasDiagonalPointAbove = point2.Y > checkAABBox.getMin().Y || point3.Y > checkAABBox.getMin().Y;

                if(hasDiagonalPointAbove || point1.Y > checkAABBox.getMin().Y)
                {
                    void *memPtr = triangleShapesPool->allocate();
                    trianglesInAABBox.emplace_back(CollisionTriangleShape(std::shared_ptr<TriangleShape3D<float>>(
                            new (memPtr) TriangleShape3D<float>(point1, point3, point2), TriangleShapeDeleter(triangleShapesPool))));
                }

                if(hasDiagonalPointAbove || point4.Y > checkAABBox.getMin().Y)
                {
                    void *memPtr = triangleShapesPool->allocate();
                    trianglesInAABBox.emplace_back(CollisionTriangleShape(std::shared_ptr<TriangleShape3D<float>>(
                            new (memPtr) TriangleShape3D<float>(point2, point3, point4), TriangleShapeDeleter(triangleShapesPool))));
                }
            }
        }

        return trianglesInAABBox;
    }

    CollisionHeightfieldShape::TriangleShapeDeleter::TriangleShapeDeleter(FixedSizePool<TriangleShape3D<float>> *const triangleShapesPool) :
            triangleShapesPool(triangleShapesPool)
    {

    }

    void CollisionHeightfieldShape::TriangleShapeDeleter::operator()(TriangleShape3D<float> *const triangleShape)
    {
        triangleShapesPool->free(triangleShape);
    }

}
