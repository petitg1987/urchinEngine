#include <vector>

#include "CollisionConvexHullReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionConvexHullReaderWriter::loadFrom(const XmlChunk* shapeChunk, const XmlParser& xmlParser) const {
        auto pointsListChunk = xmlParser.getUniqueChunk(true, POINTS_TAG, DataAttribute(), shapeChunk);
        auto pointsChunk = xmlParser.getChunks(POINT_TAG, DataAttribute(), pointsListChunk.get());

        std::vector<Point3<float>> points;
        points.reserve(pointsChunk.size());
        for (auto& pointChunk : pointsChunk) {
            points.push_back(pointChunk->getPoint3Value());
        }

        return new CollisionConvexHullShape(points);
    }

    void CollisionConvexHullReaderWriter::writeOn(XmlChunk* shapeChunk, const CollisionShape3D& collisionShape, XmlWriter& xmlWriter) const {
        shapeChunk->setAttribute(DataAttribute(TYPE_ATTR, CONVEX_HULL_VALUE));

        const auto& convexHullShape = dynamic_cast<const CollisionConvexHullShape&>(collisionShape);

        auto pointsListChunk = xmlWriter.createChunk(POINTS_TAG, DataAttribute(), shapeChunk);
        const std::vector<Point3<float>>& points = convexHullShape.getPoints();
        for (const auto& point : points) {
            auto pointChunk = xmlWriter.createChunk(POINT_TAG, DataAttribute(), pointsListChunk.get());
            pointChunk->setPoint3Value(point);
        }
    }

}
