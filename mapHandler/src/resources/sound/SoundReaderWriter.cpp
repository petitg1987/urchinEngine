#include "SoundReaderWriter.h"

namespace urchin {

    std::unique_ptr<Sound> SoundReaderWriter::loadFrom(const DataChunk* soundChunk, const DataParser& dataParser) {
        auto sound = buildSoundFrom(soundChunk, dataParser);

        loadPropertiesOn(*sound, soundChunk, dataParser);

        return sound;
    }

    void SoundReaderWriter::writeOn(DataChunk* soundChunk, const Sound& sound, DataWriter& dataWriter) {
        buildChunkFrom(soundChunk, sound, dataWriter);

        writePropertiesOn(soundChunk, sound, dataWriter);
    }

    std::unique_ptr<Sound> SoundReaderWriter::buildSoundFrom(const DataChunk* soundChunk, const DataParser& dataParser) {
        auto filenameChunk = dataParser.getUniqueChunk(true, FILENAME_TAG, DataAttribute(), soundChunk);
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
            auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, DataAttribute(), soundChunk);
            auto spatialSound = std::make_unique<SpatialSound>(filename, category, positionChunk->getPoint3Value());

            auto inaudibleDistanceChunk = dataParser.getUniqueChunk(true, INAUDIBLE_DISTANCE_TAG, DataAttribute(), soundChunk);
            spatialSound->setInaudibleDistance(inaudibleDistanceChunk->getFloatValue());

            return spatialSound;
        } else if (soundType == GLOBAL_VALUE) {
            return std::make_unique<GlobalSound>(filename, category);
        } else {
            throw std::invalid_argument("Unknown sound type read from map: " + soundType);
        }
    }

    void SoundReaderWriter::buildChunkFrom(DataChunk* soundChunk, const Sound& sound, DataWriter& dataWriter) {
        auto filenameChunk = dataWriter.createChunk(FILENAME_TAG, DataAttribute(), soundChunk);
        filenameChunk->setStringValue(sound.getFilename());

        if (sound.getSoundType() == Sound::SPATIAL) {
            const auto& spatialSound = dynamic_cast<const SpatialSound&>(sound);
            soundChunk->addAttribute(DataAttribute(TYPE_ATTR, SPATIAL_VALUE));

            auto positionChunk = dataWriter.createChunk(POSITION_TAG, DataAttribute(), soundChunk);
            positionChunk->setPoint3Value(spatialSound.getPosition());

            auto inaudibleDistanceChunk = dataWriter.createChunk(INAUDIBLE_DISTANCE_TAG, DataAttribute(), soundChunk);
            inaudibleDistanceChunk->setFloatValue(spatialSound.getInaudibleDistance());
        } else if (sound.getSoundType() == Sound::GLOBAL) {
            soundChunk->addAttribute(DataAttribute(TYPE_ATTR, GLOBAL_VALUE));
        } else {
            throw std::invalid_argument("Unknown sound type to write in map: " + std::to_string(sound.getSoundType()));
        }

        if (sound.getSoundCategory() == Sound::SoundCategory::MUSIC) {
            soundChunk->addAttribute(DataAttribute(CATEGORY_ATTR, MUSIC_VALUE));
        } else if (sound.getSoundCategory() == Sound::SoundCategory::EFFECTS) {
            soundChunk->addAttribute(DataAttribute(CATEGORY_ATTR, EFFECTS_VALUE));
        } else {
            throw std::invalid_argument("Unknown sound category to write in map: " + std::to_string(sound.getSoundCategory()));
        }
    }

    void SoundReaderWriter::loadPropertiesOn(Sound& sound, const DataChunk* soundChunk, const DataParser& dataParser) {
        auto initialVolumeChunk = dataParser.getUniqueChunk(true, INITIAL_VOLUME_TAG, DataAttribute(), soundChunk);
        sound.setInitialVolume(initialVolumeChunk->getFloatValue());
    }

    void SoundReaderWriter::writePropertiesOn(const DataChunk* soundChunk, const Sound& sound, DataWriter& dataWriter) {
        auto initialVolumeChunk = dataWriter.createChunk(INITIAL_VOLUME_TAG, DataAttribute(), soundChunk);
        initialVolumeChunk->setFloatValue(sound.getInitialVolume());
    }

}
