#include <vector>

#include <map/save/object/bodyshape/CollisionConvexHullReaderWriter.h>

namespace urchin {

    std::unique_ptr<CollisionShape3D> CollisionConvexHullReaderWriter::load(const UdaChunk* shapeChunk, const UdaParser& udaParser) const {
        auto pointsListChunk = udaParser.getFirstChunk(true, POINTS_TAG, UdaAttribute(), shapeChunk);
        auto pointsChunk = udaParser.getChunks(POINT_TAG, UdaAttribute(), pointsListChunk);

        std::vector<Point3<float>> points;
        points.reserve(pointsChunk.size());
        for (auto& pointChunk : pointsChunk) {
            points.push_back(pointChunk->getPoint3Value());
        }

        return std::make_unique<CollisionConvexHullShape>(points);
    }

    void CollisionConvexHullReaderWriter::write(UdaChunk& shapeChunk, const CollisionShape3D& collisionShape, UdaParser& udaParser) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, CONVEX_HULL_VALUE));

        const auto& convexHullShape = static_cast<const CollisionConvexHullShape&>(collisionShape);

        auto& pointsListChunk = udaParser.createChunk(POINTS_TAG, UdaAttribute(), &shapeChunk);
        const std::vector<Point3<float>>& points = convexHullShape.getPoints();
        for (const auto& point : points) {
            auto& pointChunk = udaParser.createChunk(POINT_TAG, UdaAttribute(), &pointsListChunk);
            pointChunk.setPoint3Value(point);
        }
    }

}
