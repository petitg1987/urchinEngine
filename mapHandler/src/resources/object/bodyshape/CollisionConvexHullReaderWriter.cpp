#include <vector>

#include "CollisionConvexHullReaderWriter.h"

namespace urchin
{

	CollisionConvexHullReaderWriter::CollisionConvexHullReaderWriter()
	{

	}

	CollisionConvexHullReaderWriter::~CollisionConvexHullReaderWriter()
	{

	}

	CollisionShape3D *CollisionConvexHullReaderWriter::loadFrom(std::shared_ptr<XmlChunk> shapeChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> pointsListChunk = xmlParser.getUniqueChunk(true, POINTS_TAG, XmlAttribute(), shapeChunk);
		std::vector<std::shared_ptr<XmlChunk>> pointsChunk = xmlParser.getChunks(POINT_TAG, XmlAttribute(), pointsListChunk);

		std::vector<Point3<float>> points;
		for(unsigned int i=0; i<pointsChunk.size(); ++i)
		{
			Point3<float> point = pointsChunk[i]->getPoint3Value();
			points.push_back(point);

		}

		CollisionShape3D *collisionShape = new CollisionConvexHullShape(points);
		return collisionShape;
	}

	void CollisionConvexHullReaderWriter::writeOn(std::shared_ptr<XmlChunk> shapeChunk, const CollisionShape3D *collisionShape, XmlWriter &xmlWriter) const
	{
		shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, CONVEX_HULL_VALUE));

		const CollisionConvexHullShape *convexHullShape = static_cast<const CollisionConvexHullShape *>(collisionShape);

		std::shared_ptr<XmlChunk> pointsListChunk = xmlWriter.createChunk(POINTS_TAG, XmlAttribute(), shapeChunk);
		const std::vector<Point3<float>> &points = convexHullShape->getPoints();
		for(unsigned int i=0; i<points.size(); ++i)
		{
			std::shared_ptr<XmlChunk> pointChunk = xmlWriter.createChunk(POINT_TAG, XmlAttribute(), pointsListChunk);
			pointChunk->setPoint3Value(points[i]);
		}
	}

}
