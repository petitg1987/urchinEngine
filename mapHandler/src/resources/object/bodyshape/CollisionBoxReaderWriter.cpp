#include "CollisionBoxReaderWriter.h"

namespace urchin
{

	CollisionBoxReaderWriter::CollisionBoxReaderWriter()
	{

	}

	CollisionBoxReaderWriter::~CollisionBoxReaderWriter()
	{

	}

	CollisionShape3D *CollisionBoxReaderWriter::loadFrom(std::shared_ptr<XmlChunk> shapeChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> halfSizeChunk = xmlParser.getUniqueChunk(true, HALF_SIZE_TAG, XmlAttribute(), shapeChunk);
		Vector3<float> halfSize = halfSizeChunk->getVector3Value();

		CollisionShape3D *collisionShape = new CollisionBoxShape(halfSize);
		return collisionShape;
	}

	void CollisionBoxReaderWriter::writeOn(std::shared_ptr<XmlChunk> shapeChunk, const CollisionShape3D *collisionShape, XmlWriter &xmlWriter) const
	{
		shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, BOX_VALUE));

		const CollisionBoxShape *boxShape = static_cast<const CollisionBoxShape *>(collisionShape);

		std::shared_ptr<XmlChunk> halfSizeChunk = xmlWriter.createChunk(HALF_SIZE_TAG, XmlAttribute(), shapeChunk);
		halfSizeChunk->setVector3Value(boxShape->getHalfSizes());
	}

}
