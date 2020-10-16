#include "SoundTriggerReaderWriter.h"
#include "resources/sound/soundshape/SoundShapeReaderWriterRetriever.h"
#include "resources/sound/soundshape/SoundShapeReaderWriter.h"

namespace urchin {

    SoundTrigger *SoundTriggerReaderWriter::loadFrom(const std::shared_ptr<XmlChunk> &soundTriggerChunk, const XmlParser &xmlParser) const {
        SoundTrigger *soundTrigger = buildSoundTriggerFrom(soundTriggerChunk, xmlParser);

        return soundTrigger;
    }

    void SoundTriggerReaderWriter::writeOn(const std::shared_ptr<XmlChunk> &soundTriggerChunk, const SoundTrigger *soundTrigger, XmlWriter &xmlWriter) const {
        buildChunkFrom(soundTriggerChunk, soundTrigger, xmlWriter);
    }

    SoundTrigger *SoundTriggerReaderWriter::buildSoundTriggerFrom(const std::shared_ptr<XmlChunk> &soundTriggerChunk, const XmlParser &xmlParser) const {
        std::shared_ptr<XmlChunk> soundBehaviorChunk = xmlParser.getUniqueChunk(true, SOUND_BEHAVIOR_TAG, XmlAttribute(), soundTriggerChunk);
        SoundBehavior soundBehavior = buildSoundBehaviorFrom(soundBehaviorChunk, xmlParser);

        std::string soundTriggerType = soundTriggerChunk->getAttributeValue(TYPE_ATTR);
        if(soundTriggerType == MANUAL_VALUE) {
            return new ManualTrigger(soundBehavior);
        } else if(soundTriggerType == SHAPE_VALUE) {
            std::shared_ptr<XmlChunk> soundShapeChunk = xmlParser.getUniqueChunk(true, SOUND_SHAPE_TAG, XmlAttribute(), soundTriggerChunk);
            std::shared_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(soundShapeChunk);
            SoundShape *soundShape = soundShapeReaderWriter->loadFrom(soundShapeChunk, xmlParser);

            return new ShapeTrigger(soundBehavior, soundShape);
        }

        throw std::invalid_argument("Unknown sound trigger type read from map: " + soundTriggerType);
    }

    void SoundTriggerReaderWriter::buildChunkFrom(const std::shared_ptr<XmlChunk> &soundTriggerChunk, const SoundTrigger *soundTrigger, XmlWriter &xmlWriter) const {
        if(soundTrigger->getTriggerType()==SoundTrigger::MANUAL_TRIGGER) {
            soundTriggerChunk->setAttribute(XmlAttribute(TYPE_ATTR, MANUAL_VALUE));
        } else if(soundTrigger->getTriggerType()==SoundTrigger::SHAPE_TRIGGER) {
            const auto *shapeTrigger = dynamic_cast<const ShapeTrigger *>(soundTrigger);
            soundTriggerChunk->setAttribute(XmlAttribute(TYPE_ATTR, SHAPE_VALUE));

            std::shared_ptr<XmlChunk> soundShapeChunk = xmlWriter.createChunk(SOUND_SHAPE_TAG, XmlAttribute(), soundTriggerChunk);
            std::shared_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeTrigger->getSoundShape());
            soundShapeReaderWriter->writeOn(soundShapeChunk, shapeTrigger->getSoundShape(), xmlWriter);
        } else {
            throw std::invalid_argument("Unknown sound trigger type to write in map: " + std::to_string(soundTrigger->getTriggerType()));
        }

        std::shared_ptr<XmlChunk> soundBehaviorChunk = xmlWriter.createChunk(SOUND_BEHAVIOR_TAG, XmlAttribute(), soundTriggerChunk);
        buildChunkFrom(soundBehaviorChunk, soundTrigger->getSoundBehavior(), xmlWriter);
    }

    SoundBehavior SoundTriggerReaderWriter::buildSoundBehaviorFrom(const std::shared_ptr<XmlChunk> &soundBehaviorChunk, const XmlParser &xmlParser) const {
        std::shared_ptr<XmlChunk> playBehaviorChunk = xmlParser.getUniqueChunk(true, PLAY_BEHAVIOR_TAG, XmlAttribute(), soundBehaviorChunk);
        SoundBehavior::PlayBehavior playBehavior;
        if(playBehaviorChunk->getStringValue() == PLAY_ONCE_VALUE) {
            playBehavior = SoundBehavior::PLAY_ONCE;
        } else if(playBehaviorChunk->getStringValue() == PLAY_LOOP_VALUE) {
            playBehavior = SoundBehavior::PLAY_LOOP;
        } else {
            throw std::invalid_argument("Unknown play behavior read from map: " + playBehaviorChunk->getStringValue());
        }

        std::shared_ptr<XmlChunk> stopBehaviorChunk = xmlParser.getUniqueChunk(true, STOP_BEHAVIOR_TAG, XmlAttribute(), soundBehaviorChunk);
        SoundBehavior::StopBehavior stopBehavior;
        if(stopBehaviorChunk->getStringValue() == INSTANT_STOP_VALUE) {
            stopBehavior = SoundBehavior::INSTANT_STOP;
        } else if(stopBehaviorChunk->getStringValue() == SMOOTH_STOP_VALUE) {
            stopBehavior = SoundBehavior::SMOOTH_STOP;
        } else {
            throw std::invalid_argument("Unknown stop behavior read from map: " + stopBehaviorChunk->getStringValue());
        }

        std::shared_ptr<XmlChunk> volumeDecreasePercentageOnStopChunk = xmlParser.getUniqueChunk(false, VOLUME_DECREASE_PERCENTAGE_ON_STOP_TAG, XmlAttribute(), soundBehaviorChunk);
        if(!volumeDecreasePercentageOnStopChunk) {
            return {playBehavior, stopBehavior};
        }

        float volumeDecreasePercentageOnStop = volumeDecreasePercentageOnStopChunk->getFloatValue();
        return {playBehavior, stopBehavior, volumeDecreasePercentageOnStop};
    }

    void SoundTriggerReaderWriter::buildChunkFrom(const std::shared_ptr<XmlChunk> &soundBehaviorChunk, const SoundBehavior &soundBehavior, XmlWriter &xmlWriter) const {
        std::shared_ptr<XmlChunk> playBehaviorChunk = xmlWriter.createChunk(PLAY_BEHAVIOR_TAG, XmlAttribute(), soundBehaviorChunk);
        if(soundBehavior.getPlayBehavior()==SoundBehavior::PLAY_ONCE) {
            playBehaviorChunk->setStringValue(PLAY_ONCE_VALUE);
        } else if(soundBehavior.getPlayBehavior()==SoundBehavior::PLAY_LOOP) {
            playBehaviorChunk->setStringValue(PLAY_LOOP_VALUE);
        } else {
            throw std::invalid_argument("Unknown play behavior to write in map: " + std::to_string(soundBehavior.getPlayBehavior()));
        }

        std::shared_ptr<XmlChunk> stopBehaviorChunk = xmlWriter.createChunk(STOP_BEHAVIOR_TAG, XmlAttribute(), soundBehaviorChunk);
        if(soundBehavior.getStopBehavior()==SoundBehavior::INSTANT_STOP) {
            stopBehaviorChunk->setStringValue(INSTANT_STOP_VALUE);
        } else if(soundBehavior.getStopBehavior()==SoundBehavior::SMOOTH_STOP) {
            stopBehaviorChunk->setStringValue(SMOOTH_STOP_VALUE);
        } else {
            throw std::invalid_argument("Unknown stop behavior to write in map: " + std::to_string(soundBehavior.getStopBehavior()));
        }

        std::shared_ptr<XmlChunk> volumeDecreasePercentageOnStopChunk = xmlWriter.createChunk(VOLUME_DECREASE_PERCENTAGE_ON_STOP_TAG, XmlAttribute(), soundBehaviorChunk);
        volumeDecreasePercentageOnStopChunk->setFloatValue(soundBehavior.getVolumeDecreasePercentageOnStop());
    }

}
