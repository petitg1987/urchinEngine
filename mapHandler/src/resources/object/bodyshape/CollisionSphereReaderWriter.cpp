#include "CollisionSphereReaderWriter.h"

namespace urchin
{

	CollisionSphereReaderWriter::CollisionSphereReaderWriter()
	{

	}

	CollisionSphereReaderWriter::~CollisionSphereReaderWriter()
	{

	}

	CollisionShape3D *CollisionSphereReaderWriter::loadFrom(std::shared_ptr<XmlChunk> shapeChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> radiusChunk = xmlParser.getUniqueChunk(true, RADIUS_TAG, XmlAttribute(), shapeChunk);
		float radius = radiusChunk->getFloatValue();

		return new CollisionSphereShape(radius);
	}

	void CollisionSphereReaderWriter::writeOn(std::shared_ptr<XmlChunk> shapeChunk, const CollisionShape3D *collisionShape, XmlWriter &xmlWriter) const
	{
		shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, SPHERE_VALUE));

		const CollisionSphereShape *sphereShape = static_cast<const CollisionSphereShape *>(collisionShape);

		std::shared_ptr<XmlChunk> radiusChunk = xmlWriter.createChunk(RADIUS_TAG, XmlAttribute(), shapeChunk);
		radiusChunk->setFloatValue(sphereShape->getRadius());
	}

}
