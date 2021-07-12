#include "SoundTriggerReaderWriter.h"
#include <resources/sound/soundshape/SoundShapeReaderWriterRetriever.h>
#include <resources/sound/soundshape/SoundShapeReaderWriter.h>

namespace urchin {

    std::unique_ptr<SoundTrigger> SoundTriggerReaderWriter::loadFrom(const DataChunk* soundTriggerChunk, const DataParser& dataParser) {
        auto soundTrigger = buildSoundTriggerFrom(soundTriggerChunk, dataParser);

        return soundTrigger;
    }

    void SoundTriggerReaderWriter::writeOn(DataChunk* soundTriggerChunk, const SoundTrigger& soundTrigger, DataWriter& dataWriter) {
        buildChunkFrom(soundTriggerChunk, soundTrigger, dataWriter);
    }

    std::unique_ptr<SoundTrigger> SoundTriggerReaderWriter::buildSoundTriggerFrom(const DataChunk* soundTriggerChunk, const DataParser& dataParser) {
        SoundTrigger::PlayBehavior playBehavior = loadPlayBehaviorFrom(soundTriggerChunk, dataParser);

        std::string soundTriggerType = soundTriggerChunk->getAttributeValue(TYPE_ATTR);
        if (soundTriggerType == MANUAL_VALUE) {
            return std::make_unique<ManualTrigger>(playBehavior);
        } else if (soundTriggerType == SHAPE_VALUE) {
            auto soundShapeChunk = dataParser.getUniqueChunk(true, SOUND_SHAPE_TAG, DataAttribute(), soundTriggerChunk);
            std::unique_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(soundShapeChunk.get());
            auto soundShape = soundShapeReaderWriter->loadFrom(soundShapeChunk.get(), dataParser);

            return std::make_unique<ShapeTrigger>(playBehavior, std::move(soundShape));
        }

        throw std::invalid_argument("Unknown sound trigger type read from map: " + soundTriggerType);
    }

    void SoundTriggerReaderWriter::buildChunkFrom(DataChunk* soundTriggerChunk, const SoundTrigger& soundTrigger, DataWriter& dataWriter) {
        if (soundTrigger.getTriggerType() == SoundTrigger::MANUAL_TRIGGER) {
            soundTriggerChunk->addAttribute(DataAttribute(TYPE_ATTR, MANUAL_VALUE));
        } else if (soundTrigger.getTriggerType() == SoundTrigger::SHAPE_TRIGGER) {
            const auto& shapeTrigger = dynamic_cast<const ShapeTrigger&>(soundTrigger);
            soundTriggerChunk->addAttribute(DataAttribute(TYPE_ATTR, SHAPE_VALUE));

            auto soundShapeChunk = dataWriter.createChunk(SOUND_SHAPE_TAG, DataAttribute(), soundTriggerChunk);
            std::unique_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeTrigger.getSoundShape());
            soundShapeReaderWriter->writeOn(soundShapeChunk.get(), shapeTrigger.getSoundShape(), dataWriter);
        } else {
            throw std::invalid_argument("Unknown sound trigger type to write in map: " + std::to_string(soundTrigger.getTriggerType()));
        }

        writePlayBehaviorFrom(soundTriggerChunk, soundTrigger.getPlayBehavior(), dataWriter);
    }

    SoundTrigger::PlayBehavior SoundTriggerReaderWriter::loadPlayBehaviorFrom(const DataChunk* soundTriggerChunk, const DataParser& dataParser) {
        auto playBehaviorChunk = dataParser.getUniqueChunk(true, PLAY_BEHAVIOR_TAG, DataAttribute(), soundTriggerChunk);
        if (playBehaviorChunk->getStringValue() == PLAY_ONCE_VALUE) {
            return SoundTrigger::PLAY_ONCE;
        } else if (playBehaviorChunk->getStringValue() == PLAY_LOOP_VALUE) {
            return SoundTrigger::PLAY_LOOP;
        }
        throw std::invalid_argument("Unknown play behavior read from map: " + playBehaviorChunk->getStringValue());
    }

    void SoundTriggerReaderWriter::writePlayBehaviorFrom(const DataChunk* soundTriggerChunk, SoundTrigger::PlayBehavior playBehavior, DataWriter& dataWriter) {
        auto playBehaviorChunk = dataWriter.createChunk(PLAY_BEHAVIOR_TAG, DataAttribute(), soundTriggerChunk);
        if (playBehavior == SoundTrigger::PLAY_ONCE) {
            playBehaviorChunk->setStringValue(PLAY_ONCE_VALUE);
        } else if (playBehavior == SoundTrigger::PLAY_LOOP) {
            playBehaviorChunk->setStringValue(PLAY_LOOP_VALUE);
        } else {
            throw std::invalid_argument("Unknown play behavior to write in map: " + std::to_string(playBehavior));
        }
    }

}
