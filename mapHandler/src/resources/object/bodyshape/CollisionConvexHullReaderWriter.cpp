#include <vector>

#include "CollisionConvexHullReaderWriter.h"

namespace urchin {

    CollisionShape3D* CollisionConvexHullReaderWriter::loadFrom(const std::shared_ptr<XmlChunk>& shapeChunk, const XmlParser& xmlParser) const {
        std::shared_ptr<XmlChunk> pointsListChunk = xmlParser.getUniqueChunk(true, POINTS_TAG, XmlAttribute(), shapeChunk);
        std::vector<std::shared_ptr<XmlChunk>> pointsChunk = xmlParser.getChunks(POINT_TAG, XmlAttribute(), pointsListChunk);

        std::vector<Point3<float>> points;
        points.reserve(pointsChunk.size());
        for (auto& pointChunk : pointsChunk) {
            points.push_back(pointChunk->getPoint3Value());
        }

        return new CollisionConvexHullShape(points);
    }

    void CollisionConvexHullReaderWriter::writeOn(const std::shared_ptr<XmlChunk>& shapeChunk, const CollisionShape3D& collisionShape, XmlWriter& xmlWriter) const {
        shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, CONVEX_HULL_VALUE));

        const auto& convexHullShape = dynamic_cast<const CollisionConvexHullShape&>(collisionShape);

        std::shared_ptr<XmlChunk> pointsListChunk = xmlWriter.createChunk(POINTS_TAG, XmlAttribute(), shapeChunk);
        const std::vector<Point3<float>>& points = convexHullShape.getPoints();
        for (const auto& point : points) {
            std::shared_ptr<XmlChunk> pointChunk = xmlWriter.createChunk(POINT_TAG, XmlAttribute(), pointsListChunk);
            pointChunk->setPoint3Value(point);
        }
    }

}
