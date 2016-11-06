#include <vector>

#include "CollisionCompoundShapeReaderWriter.h"
#include "CollisionShapeReaderWriterRetriever.h"
#include "resources/common/OrientationReaderWriter.h"

namespace urchin
{

	CollisionCompoundShapeReaderWriter::CollisionCompoundShapeReaderWriter()
	{

	}

	CollisionCompoundShapeReaderWriter::~CollisionCompoundShapeReaderWriter()
	{

	}

	CollisionShape3D *CollisionCompoundShapeReaderWriter::loadFrom(std::shared_ptr<XmlChunk> shapeChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> localizedShapesListChunk = xmlParser.getUniqueChunk(true, LOCALIZED_SHAPES, XmlAttribute(), shapeChunk);
		std::vector<std::shared_ptr<XmlChunk>> localizedShapesChunk = xmlParser.getChunks(LOCALIZED_SHAPE, XmlAttribute(), localizedShapesListChunk);

		std::vector<std::shared_ptr<const LocalizedCollisionShape>> compoundShapes;
		for(unsigned int i=0; i<localizedShapesChunk.size(); ++i)
		{
			std::shared_ptr<LocalizedCollisionShape> localizedShape(new LocalizedCollisionShape());

			localizedShape->position = i;

			loadTransformOn(localizedShape, localizedShapesChunk[i], xmlParser);

			std::shared_ptr<XmlChunk> shapeChunk = xmlParser.getUniqueChunk(true, SHAPE, XmlAttribute(), localizedShapesChunk[i]);
			CollisionShape3D *embeddedCollisionShape = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeChunk)->loadFrom(shapeChunk, xmlParser);
			localizedShape->shape = std::shared_ptr<CollisionShape3D>(embeddedCollisionShape);

			compoundShapes.push_back(localizedShape);
		}

		CollisionShape3D *collisionShape = new CollisionCompoundShape(compoundShapes);
		return collisionShape;
	}

	void CollisionCompoundShapeReaderWriter::writeOn(std::shared_ptr<XmlChunk> shapeChunk, const CollisionShape3D *collisionShape, XmlWriter &xmlWriter) const
	{
		shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, COMPOUND_SHAPE_VALUE));

		const CollisionCompoundShape *compoundShape = static_cast<const CollisionCompoundShape *>(collisionShape);

		std::shared_ptr<XmlChunk> localizedShapesListChunk = xmlWriter.createChunk(LOCALIZED_SHAPES, XmlAttribute(), shapeChunk);
		const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &shapes = compoundShape->getLocalizedShapes();
		for(unsigned int i=0; i<shapes.size(); ++i)
		{
			std::shared_ptr<XmlChunk> localizedShapeChunk = xmlWriter.createChunk(LOCALIZED_SHAPE, XmlAttribute(), localizedShapesListChunk);

			writeTransformOn(localizedShapeChunk, shapes[i], xmlWriter);

			std::shared_ptr<XmlChunk> shapeChunk = xmlWriter.createChunk(SHAPE, XmlAttribute(), localizedShapeChunk);
			const CollisionShape3D *collisionShape = shapes[i]->shape.get();
			CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(collisionShape)->writeOn(shapeChunk, collisionShape, xmlWriter);
		}
	}

	void CollisionCompoundShapeReaderWriter::loadTransformOn(std::shared_ptr<LocalizedCollisionShape> localizedShape, std::shared_ptr<XmlChunk> localizedShapeChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> transformChunk = xmlParser.getUniqueChunk(true, TRANSFORM_TAG, XmlAttribute(), localizedShapeChunk);

		std::shared_ptr<XmlChunk> positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), transformChunk);
		Point3<float> position = positionChunk->getPoint3Value();

		Quaternion<float> orientation = OrientationReaderWriter().loadOrientation(transformChunk, xmlParser);

		localizedShape->transform = PhysicsTransform(position, orientation);
	}

	void CollisionCompoundShapeReaderWriter::writeTransformOn(std::shared_ptr<XmlChunk> localizedShapeChunk, std::shared_ptr<const LocalizedCollisionShape> localizedShape, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> transformChunk = xmlWriter.createChunk(TRANSFORM_TAG, XmlAttribute(), localizedShapeChunk);

		std::shared_ptr<XmlChunk> positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), transformChunk);
		positionChunk->setPoint3Value(localizedShape->transform.getPosition());

		OrientationReaderWriter().writeOrientation(transformChunk, localizedShape->transform.getOrientation(), xmlWriter);
	}

}
