#include <limits>
#include <cassert>
#include <algorithm>

#include <shape/CollisionHeightfieldShape.h>

namespace urchin {

    CollisionHeightfieldShape::CollisionHeightfieldShape(std::vector<Point3<float>> vertices, unsigned int xLength, unsigned int zLength) :
            vertices(std::move(vertices)),
            xLength(xLength),
            zLength(zLength),
            localAABBox(buildLocalAABBox()) {
        assert(this->vertices.size() == xLength * zLength);
    }

    CollisionHeightfieldShape::~CollisionHeightfieldShape() {
        trianglesInAABBox.clear();
    }

    BoxShape<float> CollisionHeightfieldShape::buildLocalAABBox() const {
        Point3 min(vertices[0].X, std::numeric_limits<float>::max(), vertices[0].Z);
        Point3 max(vertices[xLength - 1].X, -std::numeric_limits<float>::max(), vertices[vertices.size() - 1].Z);

        for (const auto& vertex : vertices) {
            if (min.Y > vertex.Y) {
                min.Y = vertex.Y;
            }
            if (max.Y < vertex.Y) {
                max.Y = vertex.Y;
            }
        }

        //center on Y axis
        float maxAbsoluteYValue = std::max(std::abs(min.Y), std::abs(max.Y));
        min.Y = -maxAbsoluteYValue;
        max.Y = maxAbsoluteYValue;

        assert(std::abs(std::abs(min.X) - max.X) < 0.01f);
        assert(std::abs(std::abs(min.Y) - max.Y) < 0.01f);
        assert(std::abs(std::abs(min.Z) - max.Z) < 0.01f);

        Vector3 halfSizes((max.X - min.X) / 2.0f, (max.Y - min.Y) / 2.0f, (max.Z - min.Z) / 2.0f);
        return BoxShape(halfSizes);
    }

    CollisionShape3D::ShapeType CollisionHeightfieldShape::getShapeType() const {
        return HEIGHTFIELD_SHAPE;
    }

    const ConvexShape3D<float>& CollisionHeightfieldShape::getSingleShape() const {
        throw std::runtime_error("Impossible to retrieve single convex shape for heightfield shape");
    }

    std::unique_ptr<CollisionShape3D> CollisionHeightfieldShape::scale(const Vector3<float>& scale) const {
        if (!MathFunction::isOne(scale.X, 0.001f) || !MathFunction::isOne(scale.Y, 0.001f) || !MathFunction::isOne(scale.Z, 0.001f)) {
            throw std::runtime_error("Scaling a heightfield shape is currently not supported");
        }

        return std::make_unique<CollisionHeightfieldShape>(vertices, xLength, zLength);
    }

    const std::vector<Point3<float>>& CollisionHeightfieldShape::getVertices() const {
        return vertices;
    }

    unsigned int CollisionHeightfieldShape::getXLength() const {
        return xLength;
    }

    unsigned int CollisionHeightfieldShape::getZLength() const {
        return zLength;
    }

    AABBox<float> CollisionHeightfieldShape::toAABBox(const PhysicsTransform& physicsTransform) const {
        const Matrix3<float>& orientation = physicsTransform.retrieveOrientationMatrix();
        Point3<float> extend(
                localAABBox.getHalfSize(0) * std::abs(orientation(0)) + localAABBox.getHalfSize(1) * std::abs(orientation(3)) + localAABBox.getHalfSize(2) * std::abs(orientation(6)),
                localAABBox.getHalfSize(0) * std::abs(orientation(1)) + localAABBox.getHalfSize(1) * std::abs(orientation(4)) + localAABBox.getHalfSize(2) * std::abs(orientation(7)),
                localAABBox.getHalfSize(0) * std::abs(orientation(2)) + localAABBox.getHalfSize(1) * std::abs(orientation(5)) + localAABBox.getHalfSize(2) * std::abs(orientation(8))
        );

        const Point3<float>& position = physicsTransform.getPosition();
        return AABBox(position - extend, position + extend);
    }

    std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> CollisionHeightfieldShape::toConvexObject(const PhysicsTransform&) const {
        throw std::runtime_error("Impossible to transform heightfield shape to convex object");
    }

    Vector3<float> CollisionHeightfieldShape::computeLocalInertia(float mass) const {
        AABBox<float> aabbox = toAABBox(PhysicsTransform());
        float width = 2.0f * aabbox.getHalfSize(0);
        float height = 2.0f * aabbox.getHalfSize(1);
        float depth = 2.0f * aabbox.getHalfSize(2);

        float localInertia1 = (1.0f / 12.0f) * mass * (height * height + depth * depth);
        float localInertia2 = (1.0f / 12.0f) * mass * (width * width + depth * depth);
        float localInertia3 = (1.0f / 12.0f) * mass * (width * width + height * height);
        return Vector3(localInertia1, localInertia2, localInertia3);
    }

    float CollisionHeightfieldShape::getMaxDistanceToCenter() const {
        throw std::runtime_error("Impossible to get max distance to center for heightfield shape. A heightfield body must be static.");
    }

    float CollisionHeightfieldShape::getMinDistanceToCenter() const {
        return 0.0f;
    }

    std::unique_ptr<CollisionShape3D> CollisionHeightfieldShape::clone() const {
        return std::make_unique<CollisionHeightfieldShape>(vertices, xLength, zLength);
    }

    const std::vector<CollisionTriangleShape>& CollisionHeightfieldShape::findTrianglesInAABBox(const AABBox<float>& checkAABBox) const {
        trianglesInAABBox.clear();

        auto [vertexXMin, vertexXMax] = computeStartEndIndices(checkAABBox.getMin().X, checkAABBox.getMax().X, X);
        auto [vertexZMin, vertexZMax] = computeStartEndIndices(checkAABBox.getMin().Z, checkAABBox.getMax().Z, Z);

        for (unsigned int z = vertexZMin; z < vertexZMax; ++z) {
            for (unsigned int x = vertexXMin; x < vertexXMax; ++x) {
                createTrianglesMatchHeight(x, z, checkAABBox.getMin().Y, checkAABBox.getMax().Y);
            }
        }

        return trianglesInAABBox;
    }

    const std::vector<CollisionTriangleShape>& CollisionHeightfieldShape::findTrianglesHitByRay(const LineSegment3D<float>& ray) const {
        trianglesInAABBox.clear();

        bool raySameXValues = ray.getA().X == ray.getB().X;
        bool raySameZValues = ray.getA().Z == ray.getB().Z;

        //2d line equation variables
        float slopeLineZX = (ray.getB().X - ray.getA().X) / (ray.getB().Z - ray.getA().Z);
        float slopeLineXY = (ray.getB().Y - ray.getA().Y) / (ray.getB().X - ray.getA().X);
        float slopeLineZY = (ray.getB().Y - ray.getA().Y) / (ray.getB().Z - ray.getA().Z);
        float interceptLineZX = ray.getA().X - (slopeLineZX * ray.getA().Z);
        float interceptLineXY = ray.getA().Y - (slopeLineXY * ray.getA().X);
        float interceptLineZY = ray.getA().Y - (slopeLineZY * ray.getA().Z);

        float rayMinZ = std::min(ray.getA().Z, ray.getB().Z);
        float rayMaxZ = std::max(ray.getA().Z, ray.getB().Z);
        auto [vertexZMin, vertexZMax] = computeStartEndIndices(rayMinZ, rayMaxZ, Z);

        for (unsigned int z = vertexZMin; z < vertexZMax; ++z) {
            float zStartValue = vertices[xLength * z].Z;
            float zEndValue = vertices[xLength * (z + 1)].Z;
            float xFirstValue = raySameZValues ? ray.getA().X : slopeLineZX * zStartValue + interceptLineZX;
            float xSecondValue = raySameZValues ? ray.getB().X : slopeLineZX * zEndValue + interceptLineZX;
            auto [xMinValue, xMaxValue] = std::minmax(xFirstValue, xSecondValue);

            auto [vertexXMin, vertexXMax] = computeStartEndIndices(xMinValue, xMaxValue, X);

            for (unsigned int x = vertexXMin; x < vertexXMax; ++x) {
                //compute min and max Y values based on X value (don't give satisfying result when ray is parallel to Z axis)
                float xCurrentValue = vertices[x].X;
                float xNextValue = vertices[x + 1].X;
                float yFirstValue1 = raySameXValues ? ray.getA().Y : slopeLineXY * xCurrentValue + interceptLineXY;
                float ySecondValue1 = raySameXValues ? ray.getB().Y : slopeLineXY * xNextValue + interceptLineXY;
                auto [yMinValue1, yMaxValue1] = std::minmax(yFirstValue1, ySecondValue1);

                //compute min and max Y values based on Z value (don't give satisfying result when ray is parallel to X axis)
                float yFirstValue2 = raySameZValues ? ray.getA().Y : slopeLineZY * zStartValue + interceptLineZY;
                float ySecondValue2 = raySameZValues ? ray.getB().Y : slopeLineZY * zEndValue + interceptLineZY;
                auto [yMinValue2, yMaxValue2] = std::minmax(yFirstValue2, ySecondValue2);

                //combine both results to get a more satisfying result
                float rayMinY = std::max(yMinValue1, yMinValue2);
                float rayMaxY = std::min(yMaxValue1, yMaxValue2);

                createTrianglesMatchHeight(x, z, rayMinY, rayMaxY);
            }
        }

        return trianglesInAABBox;
    }

    /**
     * @param minValue Lower bound value on X (or Z) axis
     * @param maxValue Upper bound value on X (or Z) axis
     */
    std::pair<unsigned int, unsigned int> CollisionHeightfieldShape::computeStartEndIndices(float minValue, float maxValue, Axis axis) const {
        float halfSize = axis == X ? localAABBox.getHalfSizes().X : localAABBox.getHalfSizes().Z;
        float verticesDistance = axis == X ? vertices[1].X - vertices[0].X : vertices[xLength].Z - vertices[0].Z;
        int maxLength = axis == X ? (int)xLength - 1 : (int)zLength - 1;

        auto rawStartVertex = (int)((minValue + halfSize) / verticesDistance);
        auto startVertex = (unsigned int)std::clamp(rawStartVertex, 0, maxLength);

        auto rawEndVertex = (int)((maxValue + halfSize) / verticesDistance) + 1;
        auto endVertex = (unsigned int)std::clamp(rawEndVertex, 0, maxLength);

        return std::make_pair(startVertex, endVertex);
    }

    void CollisionHeightfieldShape::createTrianglesMatchHeight(unsigned int x, unsigned int z, float minY, float maxY) const {
        Point3<float> point1 = vertices[x + xLength * z]; //far-left
        Point3<float> point2 = vertices[x + 1 + xLength * z]; //far-right
        Point3<float> point3 = vertices[x + xLength * (z + 1)]; //near-left
        Point3<float> point4 = vertices[x + 1 + xLength * (z + 1)]; //near-right

        bool hasDiagonalPointAbove = point2.Y > minY || point3.Y > minY;
        bool hasDiagonalPointBelow = point2.Y < maxY || point3.Y < maxY;

        if ( (point1.Y > minY || hasDiagonalPointAbove) && (point1.Y < maxY || hasDiagonalPointBelow) ) {
            createCollisionTriangleShape(point1, point3, point2);
        }

        if ( (point4.Y > minY || hasDiagonalPointAbove) && (point4.Y < maxY || hasDiagonalPointBelow) ) {
            createCollisionTriangleShape(point2, point3, point4);
        }
    }

    void CollisionHeightfieldShape::createCollisionTriangleShape(const Point3<float>& p1, const Point3<float>& p2, const Point3<float>& p3) const {
        trianglesInAABBox.emplace_back(TriangleShape3D(p1, p2, p3));
    }

}
