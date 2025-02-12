#include <map/save/object/ObjectEntityReaderWriter.h>
#include <map/save/object/physics/RigidBodyReaderWriter.h>
#include <map/save/object/light/LightReaderWriter.h>
#include <map/save/object/sound/SoundComponentReaderWriter.h>
#include <map/save/object/ModelReaderWriter.h>
#include <map/save/common/TagsReaderWriter.h>


namespace urchin {

    std::unique_ptr<ObjectEntity> ObjectEntityReaderWriter::load(const UdaChunk* objectEntityChunk, const UdaParser& udaParser) {
        auto objectEntity = std::make_unique<ObjectEntity>();

        objectEntity->setName(objectEntityChunk->getAttributeValue(NAME_ATTR));

        auto modelChunk = udaParser.getFirstChunk(true, MODEL_TAG, UdaAttribute(), objectEntityChunk);
        objectEntity->setModel(ModelReaderWriter::load(modelChunk, udaParser));

        auto physicsChunk = udaParser.getFirstChunk(false, PHYSICS_TAG, UdaAttribute(), objectEntityChunk);
        if (physicsChunk != nullptr) {
            const std::string& rigidBodyId = objectEntity->getName();
            const Transform<float>& modelTransform = objectEntity->getModel()->getTransform();
            objectEntity->setupInteractiveBody(RigidBodyReaderWriter::load(physicsChunk, rigidBodyId, modelTransform, udaParser));
        }

        auto lightChunk = udaParser.getFirstChunk(false, LIGHT_TAG, UdaAttribute(), objectEntityChunk);
        if (lightChunk != nullptr) {
            objectEntity->setLight(LightReaderWriter::load(lightChunk, udaParser));
        }

        auto soundComponentChunk = udaParser.getFirstChunk(false, SOUND_COMPONENT_TAG, UdaAttribute(), objectEntityChunk);
        if (soundComponentChunk != nullptr) {
            objectEntity->setSoundComponent(SoundComponentReaderWriter::load(soundComponentChunk, udaParser));
        }

        auto tagsChunk = udaParser.getFirstChunk(false, TAGS_TAG, UdaAttribute(), objectEntityChunk);
        if (tagsChunk != nullptr) {
            objectEntity->addTags(TagsReaderWriter::load(tagsChunk, udaParser));
        }

        return objectEntity;
    }

    void ObjectEntityReaderWriter::write(UdaChunk& objectEntityChunk, const ObjectEntity& objectEntity, UdaParser& udaParser) {
        objectEntityChunk.addAttribute(UdaAttribute(NAME_ATTR, objectEntity.getName()));

        auto& modelChunk = udaParser.createChunk(MODEL_TAG, UdaAttribute(), &objectEntityChunk);
        ModelReaderWriter::write(modelChunk, *objectEntity.getModel(), udaParser);

        if (objectEntity.getRigidBody()) {
            auto& physicsChunk = udaParser.createChunk(PHYSICS_TAG, UdaAttribute(), &objectEntityChunk);
            RigidBodyReaderWriter::write(physicsChunk, *objectEntity.getRigidBody(), udaParser);
        }

        if (objectEntity.getLight()) {
            auto& lightChunk = udaParser.createChunk(LIGHT_TAG, UdaAttribute(), &objectEntityChunk);
            LightReaderWriter::write(lightChunk, *objectEntity.getLight(), udaParser);
        }

        if (objectEntity.getSoundComponent()) {
            auto& soundComponentChunk = udaParser.createChunk(SOUND_COMPONENT_TAG, UdaAttribute(), &objectEntityChunk);
            SoundComponentReaderWriter::write(soundComponentChunk, *objectEntity.getSoundComponent(), udaParser);
        }

        if (!objectEntity.getTags().empty()) {
            auto& tagsChunk = udaParser.createChunk(TAGS_TAG, UdaAttribute(), &objectEntityChunk);
            TagsReaderWriter::write(tagsChunk, objectEntity.getTags());
        }
    }
}
