#include "CollisionSphereReaderWriter.h"

namespace urchin
{

	CollisionShape3D *CollisionSphereReaderWriter::loadFrom(const std::shared_ptr<XmlChunk> &shapeChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> radiusChunk = xmlParser.getUniqueChunk(true, RADIUS_TAG, XmlAttribute(), shapeChunk);
		float radius = radiusChunk->getFloatValue();

		return new CollisionSphereShape(radius);
	}

	void CollisionSphereReaderWriter::writeOn(const std::shared_ptr<XmlChunk> &shapeChunk, const CollisionShape3D *collisionShape, XmlWriter &xmlWriter) const
	{
		shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, SPHERE_VALUE));

		const auto *sphereShape = dynamic_cast<const CollisionSphereShape *>(collisionShape);

		std::shared_ptr<XmlChunk> radiusChunk = xmlWriter.createChunk(RADIUS_TAG, XmlAttribute(), shapeChunk);
		radiusChunk->setFloatValue(sphereShape->getRadius());
	}

}
