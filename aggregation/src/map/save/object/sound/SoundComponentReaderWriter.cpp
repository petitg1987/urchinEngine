#include "map/save/object/sound/SoundComponentReaderWriter.h"
#include "map/save/object/sound/SoundReaderWriter.h"
#include "map/save/object/sound/SoundTriggerReaderWriter.h"

namespace urchin {

    std::unique_ptr<SoundComponent> SoundComponentReaderWriter::load(const UdaChunk* soundEntityChunk, const UdaParser& udaParser) {
        auto soundChunk = udaParser.getFirstChunk(true, SOUND_TAG, UdaAttribute(), soundEntityChunk);
        std::shared_ptr sound = SoundReaderWriter::load(soundChunk, udaParser);

        auto soundTriggerChunk = udaParser.getFirstChunk(true, SOUND_TRIGGER_TAG, UdaAttribute(), soundEntityChunk);
        std::shared_ptr soundTrigger = SoundTriggerReaderWriter::load(soundTriggerChunk, udaParser, sound);

        return std::make_unique<SoundComponent>(sound, soundTrigger);
    }

    void SoundComponentReaderWriter::write(UdaChunk& soundEntityChunk, const SoundComponent& soundComponent, UdaParser& udaParser) {
        auto& soundChunk = udaParser.createChunk(SOUND_TAG, UdaAttribute(), &soundEntityChunk);
        SoundReaderWriter::write(soundChunk, soundComponent.getSound(), udaParser);

        auto& soundTriggerChunk = udaParser.createChunk(SOUND_TRIGGER_TAG, UdaAttribute(), &soundEntityChunk);
        SoundTriggerReaderWriter::write(soundTriggerChunk, soundComponent.getSoundTrigger(), udaParser);
    }

}
