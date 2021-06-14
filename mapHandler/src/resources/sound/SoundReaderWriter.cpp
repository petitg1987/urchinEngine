#include "SoundReaderWriter.h"

namespace urchin {

    Sound* SoundReaderWriter::loadFrom(const std::shared_ptr<XmlChunk>& soundChunk, const XmlParser& xmlParser) {
        Sound* sound = buildSoundFrom(soundChunk, xmlParser);

        loadPropertiesOn(sound, soundChunk, xmlParser);

        return sound;
    }

    void SoundReaderWriter::writeOn(const std::shared_ptr<XmlChunk>& soundChunk, const Sound* sound, XmlWriter& xmlWriter) {
        buildChunkFrom(soundChunk, sound, xmlWriter);

        writePropertiesOn(soundChunk, sound, xmlWriter);
    }

    Sound* SoundReaderWriter::buildSoundFrom(const std::shared_ptr<XmlChunk>& soundChunk, const XmlParser& xmlParser) {
        std::shared_ptr<XmlChunk> filenameChunk = xmlParser.getUniqueChunk(true, FILENAME_TAG, XmlAttribute(), soundChunk);
        std::string filename = filenameChunk->getStringValue();

        std::string soundType = soundChunk->getAttributeValue(TYPE_ATTR);
        if (soundType == POINT_VALUE) {
            std::shared_ptr<XmlChunk> positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), soundChunk);
            auto* pointSound = new PointSound(filename, positionChunk->getPoint3Value());

            std::shared_ptr<XmlChunk> inaudibleDistanceChunk = xmlParser.getUniqueChunk(true, INAUDIBLE_DISTANCE_TAG, XmlAttribute(), soundChunk);
            pointSound->setInaudibleDistance(inaudibleDistanceChunk->getFloatValue());

            return pointSound;
        } else if (soundType == AMBIENT_VALUE) {
            return new AmbientSound(filename);
        }

        throw std::invalid_argument("Unknown sound type read from map: " + soundType);
    }

    void SoundReaderWriter::buildChunkFrom(const std::shared_ptr<XmlChunk>& soundChunk, const Sound* sound, XmlWriter& xmlWriter) {
        std::shared_ptr<XmlChunk> filenameChunk = xmlWriter.createChunk(FILENAME_TAG, XmlAttribute(), soundChunk);
        filenameChunk->setStringValue(sound->getFilename());

        if (sound->getSoundType() == Sound::POINT) {
            const auto* pointSound = dynamic_cast<const PointSound*>(sound);
            soundChunk->setAttribute(XmlAttribute(TYPE_ATTR, POINT_VALUE));

            std::shared_ptr<XmlChunk> positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), soundChunk);
            positionChunk->setPoint3Value(pointSound->getPosition());

            std::shared_ptr<XmlChunk> inaudibleDistanceChunk = xmlWriter.createChunk(INAUDIBLE_DISTANCE_TAG, XmlAttribute(), soundChunk);
            inaudibleDistanceChunk->setFloatValue(pointSound->getInaudibleDistance());
        } else if (sound->getSoundType() == Sound::AMBIENT) {
            soundChunk->setAttribute(XmlAttribute(TYPE_ATTR, AMBIENT_VALUE));
        } else {
            throw std::invalid_argument("Unknown sound type to write in map: " + std::to_string(sound->getSoundType()));
        }
    }

    void SoundReaderWriter::loadPropertiesOn(Sound* sound, const std::shared_ptr<XmlChunk>& soundChunk, const XmlParser& xmlParser) {
        std::shared_ptr<XmlChunk> initialVolumeChunk = xmlParser.getUniqueChunk(true, INITIAL_VOLUME_TAG, XmlAttribute(), soundChunk);
        sound->setInitialVolume(initialVolumeChunk->getFloatValue());
    }

    void SoundReaderWriter::writePropertiesOn(const std::shared_ptr<XmlChunk>& soundChunk, const Sound* sound, XmlWriter& xmlWriter) {
        std::shared_ptr<XmlChunk> initialVolumeChunk = xmlWriter.createChunk(INITIAL_VOLUME_TAG, XmlAttribute(), soundChunk);
        initialVolumeChunk->setFloatValue(sound->getInitialVolume());
    }

}
