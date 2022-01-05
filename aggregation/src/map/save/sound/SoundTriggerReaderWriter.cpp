#include <map/save/sound/SoundTriggerReaderWriter.h>
#include <map/save/sound/soundshape/SoundShapeReaderWriterRetriever.h>
#include <map/save/sound/soundshape/SoundShapeReaderWriter.h>

namespace urchin {

    std::unique_ptr<SoundTrigger> SoundTriggerReaderWriter::load(const UdaChunk* soundTriggerChunk, const UdaParser& udaParser) {
        PlayBehavior playBehavior = loadPlayBehavior(soundTriggerChunk, udaParser);

        std::string soundTriggerType = soundTriggerChunk->getAttributeValue(TYPE_ATTR);
        if (soundTriggerType == MANUAL_VALUE) {
            return std::make_unique<ManualTrigger>(playBehavior);
        } else if (soundTriggerType == SHAPE_VALUE) {
            auto soundShapeChunk = udaParser.getUniqueChunk(true, SOUND_SHAPE_TAG, UdaAttribute(), soundTriggerChunk);
            std::unique_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(soundShapeChunk);
            auto soundShape = soundShapeReaderWriter->load(soundShapeChunk, udaParser);

            return std::make_unique<ZoneTrigger>(playBehavior, std::move(soundShape));
        }

        throw std::invalid_argument("Unknown sound trigger type read from map: " + soundTriggerType);
    }

    void SoundTriggerReaderWriter::write(UdaChunk& soundTriggerChunk, const SoundTrigger& soundTrigger, UdaWriter& udaWriter) {
        if (soundTrigger.getTriggerType() == SoundTrigger::MANUAL_TRIGGER) {
            soundTriggerChunk.addAttribute(UdaAttribute(TYPE_ATTR, MANUAL_VALUE));
        } else if (soundTrigger.getTriggerType() == SoundTrigger::ZONE_TRIGGER) {
            const auto& zoneTrigger = static_cast<const ZoneTrigger&>(soundTrigger);
            soundTriggerChunk.addAttribute(UdaAttribute(TYPE_ATTR, SHAPE_VALUE));

            auto& soundShapeChunk = udaWriter.createChunk(SOUND_SHAPE_TAG, UdaAttribute(), &soundTriggerChunk);
            std::unique_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(zoneTrigger.getSoundShape());
            soundShapeReaderWriter->write(soundShapeChunk, zoneTrigger.getSoundShape(), udaWriter);
        } else {
            throw std::invalid_argument("Unknown sound trigger type to write in map: " + std::to_string(soundTrigger.getTriggerType()));
        }

        writePlayBehavior(soundTriggerChunk, soundTrigger.getPlayBehavior(), udaWriter);
    }

    PlayBehavior SoundTriggerReaderWriter::loadPlayBehavior(const UdaChunk* soundTriggerChunk, const UdaParser& udaParser) {
        auto playBehaviorChunk = udaParser.getUniqueChunk(true, PLAY_BEHAVIOR_TAG, UdaAttribute(), soundTriggerChunk);
        if (playBehaviorChunk->getStringValue() == PLAY_ONCE_VALUE) {
            return PlayBehavior::PLAY_ONCE;
        } else if (playBehaviorChunk->getStringValue() == PLAY_LOOP_VALUE) {
            return PlayBehavior::PLAY_LOOP;
        }
        throw std::invalid_argument("Unknown play behavior read from map: " + playBehaviorChunk->getStringValue());
    }

    void SoundTriggerReaderWriter::writePlayBehavior(UdaChunk& soundTriggerChunk, PlayBehavior playBehavior, UdaWriter& udaWriter) {
        auto& playBehaviorChunk = udaWriter.createChunk(PLAY_BEHAVIOR_TAG, UdaAttribute(), &soundTriggerChunk);
        if (playBehavior == PlayBehavior::PLAY_ONCE) {
            playBehaviorChunk.setStringValue(PLAY_ONCE_VALUE);
        } else if (playBehavior == PlayBehavior::PLAY_LOOP) {
            playBehaviorChunk.setStringValue(PLAY_LOOP_VALUE);
        } else {
            throw std::invalid_argument("Unknown play behavior to write in map: " + std::to_string((int)playBehavior));
        }
    }

}
