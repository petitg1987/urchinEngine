#include <map/save/sound/SoundEntityReaderWriter.h>
#include <map/save/sound/SoundReaderWriter.h>
#include <map/save/sound/SoundTriggerReaderWriter.h>

namespace urchin {

    std::unique_ptr<SoundEntity> SoundEntityReaderWriter::load(const UdaChunk* soundEntityChunk, const UdaParser& udaParser) {
        auto soundEntity = std::make_unique<SoundEntity>();

        soundEntity->setName(soundEntityChunk->getAttributeValue(NAME_ATTR));

        auto soundChunk = udaParser.getFirstChunk(true, SOUND_TAG, UdaAttribute(), soundEntityChunk);
        auto soundTriggerChunk = udaParser.getFirstChunk(true, SOUND_TRIGGER_TAG, UdaAttribute(), soundEntityChunk);
        std::shared_ptr sound = SoundReaderWriter::load(soundChunk, udaParser);
        std::shared_ptr soundTrigger = SoundTriggerReaderWriter::load(soundTriggerChunk, udaParser, sound);
        soundEntity->setSoundComponent(std::make_unique<SoundComponent>(sound, soundTrigger));

        return soundEntity;
    }

    void SoundEntityReaderWriter::write(UdaChunk& soundEntityChunk, const SoundEntity& soundEntity, UdaParser& udaParser) {
        soundEntityChunk.addAttribute(UdaAttribute(NAME_ATTR, soundEntity.getName()));

        auto& soundChunk = udaParser.createChunk(SOUND_TAG, UdaAttribute(), &soundEntityChunk);
        auto& soundTriggerChunk = udaParser.createChunk(SOUND_TRIGGER_TAG, UdaAttribute(), &soundEntityChunk);
        SoundReaderWriter::write(soundChunk, soundEntity.getSoundComponent()->getSound(), udaParser);
        SoundTriggerReaderWriter::write(soundTriggerChunk, soundEntity.getSoundComponent()->getSoundTrigger(), udaParser);
    }

}
