#include "SoundTriggerReaderWriter.h"
#include <resources/sound/soundshape/SoundShapeReaderWriterRetriever.h>
#include <resources/sound/soundshape/SoundShapeReaderWriter.h>

namespace urchin {

    SoundTrigger* SoundTriggerReaderWriter::loadFrom(const XmlChunk* soundTriggerChunk, const DataParser& dataParser) {
        SoundTrigger* soundTrigger = buildSoundTriggerFrom(soundTriggerChunk, dataParser);

        return soundTrigger;
    }

    void SoundTriggerReaderWriter::writeOn(XmlChunk* soundTriggerChunk, const SoundTrigger* soundTrigger, XmlWriter& xmlWriter) {
        buildChunkFrom(soundTriggerChunk, soundTrigger, xmlWriter);
    }

    SoundTrigger* SoundTriggerReaderWriter::buildSoundTriggerFrom(const XmlChunk* soundTriggerChunk, const DataParser& dataParser) {
        SoundTrigger::PlayBehavior playBehavior = loadPlayBehaviorFrom(soundTriggerChunk, dataParser);

        std::string soundTriggerType = soundTriggerChunk->getAttributeValue(TYPE_ATTR);
        if (soundTriggerType == MANUAL_VALUE) {
            return new ManualTrigger(playBehavior);
        } else if (soundTriggerType == SHAPE_VALUE) {
            auto soundShapeChunk = dataParser.getUniqueChunk(true, SOUND_SHAPE_TAG, DataAttribute(), soundTriggerChunk);
            std::shared_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(soundShapeChunk.get());
            SoundShape* soundShape = soundShapeReaderWriter->loadFrom(soundShapeChunk.get(), dataParser);

            return new ShapeTrigger(playBehavior, soundShape);
        }

        throw std::invalid_argument("Unknown sound trigger type read from map: " + soundTriggerType);
    }

    void SoundTriggerReaderWriter::buildChunkFrom(XmlChunk* soundTriggerChunk, const SoundTrigger* soundTrigger, XmlWriter& xmlWriter) {
        if (soundTrigger->getTriggerType() == SoundTrigger::MANUAL_TRIGGER) {
            soundTriggerChunk->setAttribute(DataAttribute(TYPE_ATTR, MANUAL_VALUE));
        } else if (soundTrigger->getTriggerType() == SoundTrigger::SHAPE_TRIGGER) {
            const auto* shapeTrigger = dynamic_cast<const ShapeTrigger*>(soundTrigger);
            soundTriggerChunk->setAttribute(DataAttribute(TYPE_ATTR, SHAPE_VALUE));

            auto soundShapeChunk = xmlWriter.createChunk(SOUND_SHAPE_TAG, DataAttribute(), soundTriggerChunk);
            std::shared_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeTrigger->getSoundShape());
            soundShapeReaderWriter->writeOn(soundShapeChunk.get(), shapeTrigger->getSoundShape(), xmlWriter);
        } else {
            throw std::invalid_argument("Unknown sound trigger type to write in map: " + std::to_string(soundTrigger->getTriggerType()));
        }

        writePlayBehaviorFrom(soundTriggerChunk, soundTrigger->getPlayBehavior(), xmlWriter);
    }

    SoundTrigger::PlayBehavior SoundTriggerReaderWriter::loadPlayBehaviorFrom(const XmlChunk* soundTriggerChunk, const DataParser& dataParser) {
        auto playBehaviorChunk = dataParser.getUniqueChunk(true, PLAY_BEHAVIOR_TAG, DataAttribute(), soundTriggerChunk);
        if (playBehaviorChunk->getStringValue() == PLAY_ONCE_VALUE) {
            return SoundTrigger::PLAY_ONCE;
        } else if (playBehaviorChunk->getStringValue() == PLAY_LOOP_VALUE) {
            return SoundTrigger::PLAY_LOOP;
        }
        throw std::invalid_argument("Unknown play behavior read from map: " + playBehaviorChunk->getStringValue());
    }

    void SoundTriggerReaderWriter::writePlayBehaviorFrom(const XmlChunk* soundTriggerChunk, SoundTrigger::PlayBehavior playBehavior, XmlWriter& xmlWriter) {
        auto playBehaviorChunk = xmlWriter.createChunk(PLAY_BEHAVIOR_TAG, DataAttribute(), soundTriggerChunk);
        if (playBehavior == SoundTrigger::PLAY_ONCE) {
            playBehaviorChunk->setStringValue(PLAY_ONCE_VALUE);
        } else if (playBehavior == SoundTrigger::PLAY_LOOP) {
            playBehaviorChunk->setStringValue(PLAY_LOOP_VALUE);
        } else {
            throw std::invalid_argument("Unknown play behavior to write in map: " + std::to_string(playBehavior));
        }
    }

}
