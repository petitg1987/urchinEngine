#include "SoundTriggerReaderWriter.h"
#include <resources/sound/soundshape/SoundShapeReaderWriterRetriever.h>
#include <resources/sound/soundshape/SoundShapeReaderWriter.h>

namespace urchin {

    SoundTrigger* SoundTriggerReaderWriter::loadFrom(const std::shared_ptr<XmlChunk>& soundTriggerChunk, const XmlParser& xmlParser) {
        SoundTrigger* soundTrigger = buildSoundTriggerFrom(soundTriggerChunk, xmlParser);

        return soundTrigger;
    }

    void SoundTriggerReaderWriter::writeOn(const std::shared_ptr<XmlChunk>& soundTriggerChunk, const SoundTrigger* soundTrigger, XmlWriter& xmlWriter) {
        buildChunkFrom(soundTriggerChunk, soundTrigger, xmlWriter);
    }

    SoundTrigger* SoundTriggerReaderWriter::buildSoundTriggerFrom(const std::shared_ptr<XmlChunk>& soundTriggerChunk, const XmlParser& xmlParser) {
        SoundTrigger::PlayBehavior playBehavior = loadPlayBehaviorFrom(soundTriggerChunk, xmlParser);

        std::string soundTriggerType = soundTriggerChunk->getAttributeValue(TYPE_ATTR);
        if (soundTriggerType == MANUAL_VALUE) {
            return new ManualTrigger(playBehavior);
        } else if (soundTriggerType == SHAPE_VALUE) {
            std::shared_ptr<XmlChunk> soundShapeChunk = xmlParser.getUniqueChunk(true, SOUND_SHAPE_TAG, XmlAttribute(), soundTriggerChunk);
            std::shared_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(soundShapeChunk);
            SoundShape* soundShape = soundShapeReaderWriter->loadFrom(soundShapeChunk, xmlParser);

            return new ShapeTrigger(playBehavior, soundShape);
        }

        throw std::invalid_argument("Unknown sound trigger type read from map: " + soundTriggerType);
    }

    void SoundTriggerReaderWriter::buildChunkFrom(const std::shared_ptr<XmlChunk>& soundTriggerChunk, const SoundTrigger* soundTrigger, XmlWriter& xmlWriter) {
        if (soundTrigger->getTriggerType() == SoundTrigger::MANUAL_TRIGGER) {
            soundTriggerChunk->setAttribute(XmlAttribute(TYPE_ATTR, MANUAL_VALUE));
        } else if (soundTrigger->getTriggerType() == SoundTrigger::SHAPE_TRIGGER) {
            const auto* shapeTrigger = dynamic_cast<const ShapeTrigger*>(soundTrigger);
            soundTriggerChunk->setAttribute(XmlAttribute(TYPE_ATTR, SHAPE_VALUE));

            std::shared_ptr<XmlChunk> soundShapeChunk = xmlWriter.createChunk(SOUND_SHAPE_TAG, XmlAttribute(), soundTriggerChunk);
            std::shared_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeTrigger->getSoundShape());
            soundShapeReaderWriter->writeOn(soundShapeChunk, shapeTrigger->getSoundShape(), xmlWriter);
        } else {
            throw std::invalid_argument("Unknown sound trigger type to write in map: " + std::to_string(soundTrigger->getTriggerType()));
        }

        writePlayBehaviorFrom(soundTriggerChunk, soundTrigger->getPlayBehavior(), xmlWriter);
    }

    SoundTrigger::PlayBehavior SoundTriggerReaderWriter::loadPlayBehaviorFrom(const std::shared_ptr<XmlChunk>& soundTriggerChunk, const XmlParser& xmlParser) {
        std::shared_ptr<XmlChunk> playBehaviorChunk = xmlParser.getUniqueChunk(true, PLAY_BEHAVIOR_TAG, XmlAttribute(), soundTriggerChunk);
        if (playBehaviorChunk->getStringValue() == PLAY_ONCE_VALUE) {
            return SoundTrigger::PLAY_ONCE;
        } else if (playBehaviorChunk->getStringValue() == PLAY_LOOP_VALUE) {
            return SoundTrigger::PLAY_LOOP;
        }
        throw std::invalid_argument("Unknown play behavior read from map: " + playBehaviorChunk->getStringValue());
    }

    void SoundTriggerReaderWriter::writePlayBehaviorFrom(const std::shared_ptr<XmlChunk>& soundTriggerChunk, SoundTrigger::PlayBehavior playBehavior, XmlWriter& xmlWriter) {
        std::shared_ptr<XmlChunk> playBehaviorChunk = xmlWriter.createChunk(PLAY_BEHAVIOR_TAG, XmlAttribute(), soundTriggerChunk);
        if (playBehavior == SoundTrigger::PLAY_ONCE) {
            playBehaviorChunk->setStringValue(PLAY_ONCE_VALUE);
        } else if (playBehavior == SoundTrigger::PLAY_LOOP) {
            playBehaviorChunk->setStringValue(PLAY_LOOP_VALUE);
        } else {
            throw std::invalid_argument("Unknown play behavior to write in map: " + std::to_string(playBehavior));
        }
    }

}
