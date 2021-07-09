#include "SoundReaderWriter.h"

namespace urchin {

    Sound* SoundReaderWriter::loadFrom(const XmlChunk* soundChunk, const XmlParser& xmlParser) {
        Sound* sound = buildSoundFrom(soundChunk, xmlParser);

        loadPropertiesOn(sound, soundChunk, xmlParser);

        return sound;
    }

    void SoundReaderWriter::writeOn(XmlChunk* soundChunk, const Sound* sound, XmlWriter& xmlWriter) {
        buildChunkFrom(soundChunk, sound, xmlWriter);

        writePropertiesOn(soundChunk, sound, xmlWriter);
    }

    Sound* SoundReaderWriter::buildSoundFrom(const XmlChunk* soundChunk, const XmlParser& xmlParser) {
        auto filenameChunk = xmlParser.getUniqueChunk(true, FILENAME_TAG, XmlAttribute(), soundChunk);
        std::string filename = filenameChunk->getStringValue();

        std::string soundCategory = soundChunk->getAttributeValue(CATEGORY_ATTR);
        Sound::SoundCategory category;
        if (soundCategory == MUSIC_VALUE) {
            category = Sound::SoundCategory::MUSIC;
        } else if (soundCategory == EFFECTS_VALUE) {
            category = Sound::SoundCategory::EFFECTS;
        } else {
            throw std::invalid_argument("Unknown sound category read from map: " + soundCategory);
        }

        std::string soundType = soundChunk->getAttributeValue(TYPE_ATTR);
        if (soundType == SPATIAL_VALUE) {
            auto positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), soundChunk);
            auto* spatialSound = new SpatialSound(filename, category, positionChunk->getPoint3Value());

            auto inaudibleDistanceChunk = xmlParser.getUniqueChunk(true, INAUDIBLE_DISTANCE_TAG, XmlAttribute(), soundChunk);
            spatialSound->setInaudibleDistance(inaudibleDistanceChunk->getFloatValue());

            return spatialSound;
        } else if (soundType == GLOBAL_VALUE) {
            return new GlobalSound(filename, category);
        } else {
            throw std::invalid_argument("Unknown sound type read from map: " + soundType);
        }
    }

    void SoundReaderWriter::buildChunkFrom(XmlChunk* soundChunk, const Sound* sound, XmlWriter& xmlWriter) {
        auto filenameChunk = xmlWriter.createChunk(FILENAME_TAG, XmlAttribute(), soundChunk);
        filenameChunk->setStringValue(sound->getFilename());

        if (sound->getSoundType() == Sound::SPATIAL) {
            const auto* spatialSound = dynamic_cast<const SpatialSound*>(sound);
            soundChunk->setAttribute(XmlAttribute(TYPE_ATTR, SPATIAL_VALUE));

            auto positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), soundChunk);
            positionChunk->setPoint3Value(spatialSound->getPosition());

            auto inaudibleDistanceChunk = xmlWriter.createChunk(INAUDIBLE_DISTANCE_TAG, XmlAttribute(), soundChunk);
            inaudibleDistanceChunk->setFloatValue(spatialSound->getInaudibleDistance());
        } else if (sound->getSoundType() == Sound::GLOBAL) {
            soundChunk->setAttribute(XmlAttribute(TYPE_ATTR, GLOBAL_VALUE));
        } else {
            throw std::invalid_argument("Unknown sound type to write in map: " + std::to_string(sound->getSoundType()));
        }

        if (sound->getSoundCategory() == Sound::SoundCategory::MUSIC) {
            soundChunk->setAttribute(XmlAttribute(CATEGORY_ATTR, MUSIC_VALUE));
        } else if (sound->getSoundCategory() == Sound::SoundCategory::EFFECTS) {
            soundChunk->setAttribute(XmlAttribute(CATEGORY_ATTR, EFFECTS_VALUE));
        } else {
            throw std::invalid_argument("Unknown sound category to write in map: " + std::to_string(sound->getSoundCategory()));
        }
    }

    void SoundReaderWriter::loadPropertiesOn(Sound* sound, const XmlChunk* soundChunk, const XmlParser& xmlParser) {
        auto initialVolumeChunk = xmlParser.getUniqueChunk(true, INITIAL_VOLUME_TAG, XmlAttribute(), soundChunk);
        sound->setInitialVolume(initialVolumeChunk->getFloatValue());
    }

    void SoundReaderWriter::writePropertiesOn(const XmlChunk* soundChunk, const Sound* sound, XmlWriter& xmlWriter) {
        auto initialVolumeChunk = xmlWriter.createChunk(INITIAL_VOLUME_TAG, XmlAttribute(), soundChunk);
        initialVolumeChunk->setFloatValue(sound->getInitialVolume());
    }

}
