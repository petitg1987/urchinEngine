#include <vector>

#include "CollisionCompoundShapeReaderWriter.h"
#include "CollisionShapeReaderWriterRetriever.h"
#include <resources/common/OrientationReaderWriter.h>

namespace urchin {

    CollisionShape3D* CollisionCompoundShapeReaderWriter::loadFrom(const XmlChunk* mainShapeChunk, const XmlParser& xmlParser) const {
        auto localizedShapesListChunk = xmlParser.getUniqueChunk(true, LOCALIZED_SHAPES, DataAttribute(), mainShapeChunk);
        auto localizedShapesChunk = xmlParser.getChunks(LOCALIZED_SHAPE, DataAttribute(), localizedShapesListChunk.get());

        std::vector<std::shared_ptr<const LocalizedCollisionShape>> compoundShapes;
        for (std::size_t i = 0; i < localizedShapesChunk.size(); ++i) {
            std::shared_ptr<LocalizedCollisionShape> localizedShape(new LocalizedCollisionShape());

            localizedShape->position = i;

            loadTransformOn(*localizedShape, localizedShapesChunk[i].get(), xmlParser);

            auto shapeChunk = xmlParser.getUniqueChunk(true, COMPOUND_SHAPE_TAG, DataAttribute(), localizedShapesChunk[i].get());
            CollisionShape3D* embeddedCollisionShape = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeChunk.get())->loadFrom(shapeChunk.get(), xmlParser);
            localizedShape->shape = std::unique_ptr<CollisionShape3D>(embeddedCollisionShape);

            compoundShapes.push_back(std::move(localizedShape));
        }

        return new CollisionCompoundShape(std::move(compoundShapes));
    }

    void CollisionCompoundShapeReaderWriter::writeOn(XmlChunk* mainShapeChunk, const CollisionShape3D& mainCollisionShape, XmlWriter& xmlWriter) const {
        mainShapeChunk->setAttribute(DataAttribute(TYPE_ATTR, COMPOUND_SHAPE_VALUE));

        const auto& compoundShape = dynamic_cast<const CollisionCompoundShape&>(mainCollisionShape);

        auto localizedShapesListChunk = xmlWriter.createChunk(LOCALIZED_SHAPES, DataAttribute(), mainShapeChunk);
        const std::vector<std::shared_ptr<const LocalizedCollisionShape>>& localizedShapes = compoundShape.getLocalizedShapes();
        for (const auto& localizedShape : localizedShapes) {
            auto localizedShapeChunk = xmlWriter.createChunk(LOCALIZED_SHAPE, DataAttribute(), localizedShapesListChunk.get());

            writeTransformOn(localizedShapeChunk.get(), *localizedShape, xmlWriter);

            auto shapeChunk = xmlWriter.createChunk(COMPOUND_SHAPE_TAG, DataAttribute(), localizedShapeChunk.get());
            CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(*localizedShape->shape)->writeOn(shapeChunk.get(), *localizedShape->shape, xmlWriter);
        }
    }

    void CollisionCompoundShapeReaderWriter::loadTransformOn(LocalizedCollisionShape& localizedShape, const XmlChunk* localizedShapeChunk, const XmlParser& xmlParser) {
        auto transformChunk = xmlParser.getUniqueChunk(true, TRANSFORM_TAG, DataAttribute(), localizedShapeChunk);

        auto positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, DataAttribute(), transformChunk.get());
        Point3<float> position = positionChunk->getPoint3Value();

        Quaternion<float> orientation = OrientationReaderWriter::loadOrientation(transformChunk.get(), xmlParser);

        localizedShape.transform = PhysicsTransform(position, orientation);
    }

    void CollisionCompoundShapeReaderWriter::writeTransformOn(const XmlChunk* localizedShapeChunk, const LocalizedCollisionShape& localizedShape, XmlWriter& xmlWriter) {
        auto transformChunk = xmlWriter.createChunk(TRANSFORM_TAG, DataAttribute(), localizedShapeChunk);

        auto positionChunk = xmlWriter.createChunk(POSITION_TAG, DataAttribute(), transformChunk.get());
        positionChunk->setPoint3Value(localizedShape.transform.getPosition());

        OrientationReaderWriter::writeOrientation(transformChunk.get(), localizedShape.transform.getOrientation(), xmlWriter);
    }

}
