#include <vector>

#include <map/save/object/physics/bodyshape/CollisionCompoundShapeReaderWriter.h>
#include <map/save/object/physics/bodyshape/CollisionShapeReaderWriterRetriever.h>
#include <map/save/common/OrientationReaderWriter.h>

namespace urchin {

    std::unique_ptr<CollisionShape3D> CollisionCompoundShapeReaderWriter::load(const UdaChunk* mainShapeChunk, const UdaParser& udaParser) const {
        auto localizedShapesListChunk = udaParser.getFirstChunk(true, LOCALIZED_SHAPES, UdaAttribute(), mainShapeChunk);
        auto localizedShapesChunk = udaParser.getChunks(LOCALIZED_SHAPE, UdaAttribute(), localizedShapesListChunk);

        std::vector<std::shared_ptr<const LocalizedCollisionShape>> compoundShapes;
        compoundShapes.reserve(localizedShapesChunk.size());
        for (std::size_t i = 0; i < localizedShapesChunk.size(); ++i) {
            auto localizedShape = std::make_unique<LocalizedCollisionShape>();

            localizedShape->shapeIndex = i;

            loadTransform(*localizedShape, localizedShapesChunk[i], udaParser);

            auto shapeChunk = udaParser.getFirstChunk(true, COMPOUND_SHAPE_TAG, UdaAttribute(), localizedShapesChunk[i]);
            localizedShape->shape = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeChunk)->load(shapeChunk, udaParser);

            compoundShapes.push_back(std::move(localizedShape));
        }

        return std::make_unique<CollisionCompoundShape>(std::move(compoundShapes));
    }

    void CollisionCompoundShapeReaderWriter::write(UdaChunk& mainShapeChunk, const CollisionShape3D& mainCollisionShape, UdaParser& udaParser) const {
        mainShapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, COMPOUND_SHAPE_VALUE));

        const auto& compoundShape = static_cast<const CollisionCompoundShape&>(mainCollisionShape);

        auto& localizedShapesListChunk = udaParser.createChunk(LOCALIZED_SHAPES, UdaAttribute(), &mainShapeChunk);
        const std::vector<std::shared_ptr<const LocalizedCollisionShape>>& localizedShapes = compoundShape.getLocalizedShapes();
        for (const auto& localizedShape : localizedShapes) {
            auto& localizedShapeChunk = udaParser.createChunk(LOCALIZED_SHAPE, UdaAttribute(), &localizedShapesListChunk);

            writeTransform(localizedShapeChunk, *localizedShape, udaParser);

            auto& shapeChunk = udaParser.createChunk(COMPOUND_SHAPE_TAG, UdaAttribute(), &localizedShapeChunk);
            CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(*localizedShape->shape)->write(shapeChunk, *localizedShape->shape, udaParser);
        }
    }

    void CollisionCompoundShapeReaderWriter::loadTransform(LocalizedCollisionShape& localizedShape, const UdaChunk* localizedShapeChunk, const UdaParser& udaParser) {
        auto transformChunk = udaParser.getFirstChunk(true, TRANSFORM_TAG, UdaAttribute(), localizedShapeChunk);

        auto positionChunk = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), transformChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        Quaternion<float> orientation = OrientationReaderWriter::load(transformChunk, udaParser);

        localizedShape.transform = PhysicsTransform(position, orientation);
    }

    void CollisionCompoundShapeReaderWriter::writeTransform(UdaChunk& localizedShapeChunk, const LocalizedCollisionShape& localizedShape, UdaParser& udaParser) {
        auto& transformChunk = udaParser.createChunk(TRANSFORM_TAG, UdaAttribute(), &localizedShapeChunk);

        auto& positionChunk = udaParser.createChunk(POSITION_TAG, UdaAttribute(), &transformChunk);
        positionChunk.setPoint3Value(localizedShape.transform.getPosition());

        OrientationReaderWriter::write(transformChunk, localizedShape.transform.getOrientation(), udaParser);
    }

}
