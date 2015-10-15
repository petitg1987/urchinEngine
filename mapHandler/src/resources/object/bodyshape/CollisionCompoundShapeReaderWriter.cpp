#include <vector>

#include "CollisionCompoundShapeReaderWriter.h"
#include "CollisionShapeReaderWriterRetriever.h"

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
			std::shared_ptr<LocalizedCollisionShape> localizedBox(new LocalizedCollisionShape());

			localizedBox->position = i;

			std::shared_ptr<XmlChunk> translationChunk = xmlParser.getUniqueChunk(true, TRANSLATION, XmlAttribute(), localizedShapesChunk[i]);
			localizedBox->translation = translationChunk->getVector3Value();

			std::shared_ptr<XmlChunk> shapeChunk = xmlParser.getUniqueChunk(true, SHAPE, XmlAttribute(), localizedShapesChunk[i]);
			CollisionShape3D *embeddedCollisionShape = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeChunk)->loadFrom(shapeChunk, xmlParser);
			localizedBox->shape = std::shared_ptr<CollisionShape3D>(embeddedCollisionShape);

			compoundShapes.push_back(localizedBox);
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

			std::shared_ptr<XmlChunk> translationChunk = xmlWriter.createChunk(TRANSLATION, XmlAttribute(), localizedShapeChunk);
			translationChunk->setVector3Value(shapes[i]->translation);

			std::shared_ptr<XmlChunk> shapeChunk = xmlWriter.createChunk(SHAPE, XmlAttribute(), localizedShapeChunk);
			const CollisionShape3D *collisionShape = shapes[i]->shape.get();
			CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(collisionShape)->writeOn(shapeChunk, collisionShape, xmlWriter);
		}
	}

}
