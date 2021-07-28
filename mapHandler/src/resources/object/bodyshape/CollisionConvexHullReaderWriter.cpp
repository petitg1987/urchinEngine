#include <vector>

#include <resources/object/bodyshape/CollisionConvexHullReaderWriter.h>

namespace urchin {

    CollisionShape3D* CollisionConvexHullReaderWriter::loadFrom(const UdaChunk* shapeChunk, const UdaParser& udaParser) const {
        auto pointsListChunk = udaParser.getUniqueChunk(true, POINTS_TAG, UdaAttribute(), shapeChunk);
        auto pointsChunk = udaParser.getChunks(POINT_TAG, UdaAttribute(), pointsListChunk);

        std::vector<Point3<float>> points;
        points.reserve(pointsChunk.size());
        for (auto& pointChunk : pointsChunk) {
            points.push_back(pointChunk->getPoint3Value());
        }

        return new CollisionConvexHullShape(points);
    }

    void CollisionConvexHullReaderWriter::writeOn(UdaChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaWriter& udaWriter) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, CONVEX_HULL_VALUE));

        const auto& convexHullShape = dynamic_cast<const CollisionConvexHullShape&>(collisionShape);

        auto& pointsListChunk = udaWriter.createChunk(POINTS_TAG, UdaAttribute(), &shapeChunk);
        const std::vector<Point3<float>>& points = convexHullShape.getPoints();
        for (const auto& point : points) {
            auto& pointChunk = udaWriter.createChunk(POINT_TAG, UdaAttribute(), &pointsListChunk);
            pointChunk.setPoint3Value(point);
        }
    }

}
