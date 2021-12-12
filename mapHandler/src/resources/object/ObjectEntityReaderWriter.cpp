#include <resources/object/ObjectEntityReaderWriter.h>
#include <resources/object/RigidBodyReaderWriter.h>
#include <resources/object/ModelReaderWriter.h>
#include <resources/common/TagsReaderWriter.h>

namespace urchin {

    std::unique_ptr<ObjectEntity> ObjectEntityReaderWriter::load(const UdaChunk* objectEntityChunk, const UdaParser& udaParser) {
        auto objectEntity = std::make_unique<ObjectEntity>();

        objectEntity->setName(objectEntityChunk->getAttributeValue(NAME_ATTR));

        auto modelChunk = udaParser.getUniqueChunk(true, MODEL_TAG, UdaAttribute(), objectEntityChunk);
        objectEntity->setModel(ModelReaderWriter::load(modelChunk, udaParser));

        auto physicsChunk = udaParser.getUniqueChunk(false, PHYSICS_TAG, UdaAttribute(), objectEntityChunk);
        if (physicsChunk != nullptr) {
            const std::string& rigidBodyId = objectEntity->getName();
            const Transform<float>& modelTransform = objectEntity->getModel()->getTransform();
            objectEntity->setupInteractiveBody(RigidBodyReaderWriter::load(physicsChunk, rigidBodyId, modelTransform, udaParser));
        }

        auto tagsChunk = udaParser.getUniqueChunk(false, TAGS_TAG, UdaAttribute(), objectEntityChunk);
        if (tagsChunk != nullptr) {
            objectEntity->addTags(TagsReaderWriter::load(tagsChunk, udaParser));
        }

        return objectEntity;
    }

    void ObjectEntityReaderWriter::write(UdaChunk& objectEntityChunk, const ObjectEntity& objectEntity, UdaWriter& udaWriter) {
        objectEntityChunk.addAttribute(UdaAttribute(NAME_ATTR, objectEntity.getName()));

        auto& modelChunk = udaWriter.createChunk(MODEL_TAG, UdaAttribute(), &objectEntityChunk);
        ModelReaderWriter::write(modelChunk, *objectEntity.getModel(), udaWriter);

        if (objectEntity.getRigidBody()) {
            auto& physicsChunk = udaWriter.createChunk(PHYSICS_TAG, UdaAttribute(), &objectEntityChunk);
            RigidBodyReaderWriter::write(physicsChunk, *objectEntity.getRigidBody(), udaWriter);
        }

        if (!objectEntity.getTags().empty()) {
            auto& tagsChunk = udaWriter.createChunk(TAGS_TAG, UdaAttribute(), &objectEntityChunk);
            TagsReaderWriter::write(tagsChunk, objectEntity.getTags(), udaWriter);
        }
    }
}
