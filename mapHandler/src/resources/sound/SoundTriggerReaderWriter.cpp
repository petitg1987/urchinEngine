#include <resources/sound/SoundTriggerReaderWriter.h>
#include <resources/sound/soundshape/SoundShapeReaderWriterRetriever.h>
#include <resources/sound/soundshape/SoundShapeReaderWriter.h>

namespace urchin {

    std::unique_ptr<SoundTrigger> SoundTriggerReaderWriter::loadFrom(const UdaChunk* soundTriggerChunk, const UdaParser& udaParser) {
        auto soundTrigger = buildSoundTriggerFrom(soundTriggerChunk, udaParser);

        return soundTrigger;
    }

    void SoundTriggerReaderWriter::writeOn(UdaChunk& soundTriggerChunk, const SoundTrigger& soundTrigger, UdaWriter& udaWriter) {
        buildChunkFrom(soundTriggerChunk, soundTrigger, udaWriter);
    }

    std::unique_ptr<SoundTrigger> SoundTriggerReaderWriter::buildSoundTriggerFrom(const UdaChunk* soundTriggerChunk, const UdaParser& udaParser) {
        SoundTrigger::PlayBehavior playBehavior = loadPlayBehaviorFrom(soundTriggerChunk, udaParser);

        std::string soundTriggerType = soundTriggerChunk->getAttributeValue(TYPE_ATTR);
        if (soundTriggerType == MANUAL_VALUE) {
            return std::make_unique<ManualTrigger>(playBehavior);
        } else if (soundTriggerType == SHAPE_VALUE) {
            auto soundShapeChunk = udaParser.getUniqueChunk(true, SOUND_SHAPE_TAG, UdaAttribute(), soundTriggerChunk);
            std::unique_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(soundShapeChunk);
            auto soundShape = soundShapeReaderWriter->loadFrom(soundShapeChunk, udaParser);

            return std::make_unique<ShapeTrigger>(playBehavior, std::move(soundShape));
        }

        throw std::invalid_argument("Unknown sound trigger type read from map: " + soundTriggerType);
    }

    void SoundTriggerReaderWriter::buildChunkFrom(UdaChunk& soundTriggerChunk, const SoundTrigger& soundTrigger, UdaWriter& udaWriter) {
        if (soundTrigger.getTriggerType() == SoundTrigger::MANUAL_TRIGGER) {
            soundTriggerChunk.addAttribute(UdaAttribute(TYPE_ATTR, MANUAL_VALUE));
        } else if (soundTrigger.getTriggerType() == SoundTrigger::SHAPE_TRIGGER) {
            const auto& shapeTrigger = static_cast<const ShapeTrigger&>(soundTrigger);
            soundTriggerChunk.addAttribute(UdaAttribute(TYPE_ATTR, SHAPE_VALUE));

            auto& soundShapeChunk = udaWriter.createChunk(SOUND_SHAPE_TAG, UdaAttribute(), &soundTriggerChunk);
            std::unique_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeTrigger.getSoundShape());
            soundShapeReaderWriter->writeOn(soundShapeChunk, shapeTrigger.getSoundShape(), udaWriter);
        } else {
            throw std::invalid_argument("Unknown sound trigger type to write in map: " + std::to_string(soundTrigger.getTriggerType()));
        }

        writePlayBehaviorFrom(soundTriggerChunk, soundTrigger.getPlayBehavior(), udaWriter);
    }

    SoundTrigger::PlayBehavior SoundTriggerReaderWriter::loadPlayBehaviorFrom(const UdaChunk* soundTriggerChunk, const UdaParser& udaParser) {
        auto playBehaviorChunk = udaParser.getUniqueChunk(true, PLAY_BEHAVIOR_TAG, UdaAttribute(), soundTriggerChunk);
        if (playBehaviorChunk->getStringValue() == PLAY_ONCE_VALUE) {
            return SoundTrigger::PLAY_ONCE;
        } else if (playBehaviorChunk->getStringValue() == PLAY_LOOP_VALUE) {
            return SoundTrigger::PLAY_LOOP;
        }
        throw std::invalid_argument("Unknown play behavior read from map: " + playBehaviorChunk->getStringValue());
    }

    void SoundTriggerReaderWriter::writePlayBehaviorFrom(UdaChunk& soundTriggerChunk, SoundTrigger::PlayBehavior playBehavior, UdaWriter& udaWriter) {
        auto& playBehaviorChunk = udaWriter.createChunk(PLAY_BEHAVIOR_TAG, UdaAttribute(), &soundTriggerChunk);
        if (playBehavior == SoundTrigger::PLAY_ONCE) {
            playBehaviorChunk.setStringValue(PLAY_ONCE_VALUE);
        } else if (playBehavior == SoundTrigger::PLAY_LOOP) {
            playBehaviorChunk.setStringValue(PLAY_LOOP_VALUE);
        } else {
            throw std::invalid_argument("Unknown play behavior to write in map: " + std::to_string(playBehavior));
        }
    }

}
