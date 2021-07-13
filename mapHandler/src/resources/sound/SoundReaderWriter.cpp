#include "SoundReaderWriter.h"

namespace urchin {

    std::unique_ptr<Sound> SoundReaderWriter::loadFrom(const UdaChunk* soundChunk, const UdaParser& udaParser) {
        auto sound = buildSoundFrom(soundChunk, udaParser);

        loadPropertiesOn(*sound, soundChunk, udaParser);

        return sound;
    }

    void SoundReaderWriter::writeOn(UdaChunk& soundChunk, const Sound& sound, UdaWriter& udaWriter) {
        buildChunkFrom(soundChunk, sound, udaWriter);

        writePropertiesOn(soundChunk, sound, udaWriter);
    }

    std::unique_ptr<Sound> SoundReaderWriter::buildSoundFrom(const UdaChunk* soundChunk, const UdaParser& udaParser) {
        auto filenameChunk = udaParser.getUniqueChunk(true, FILENAME_TAG, UdaAttribute(), soundChunk);
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
            auto positionChunk = udaParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), soundChunk);
            auto spatialSound = std::make_unique<SpatialSound>(filename, category, positionChunk->getPoint3Value());

            auto inaudibleDistanceChunk = udaParser.getUniqueChunk(true, INAUDIBLE_DISTANCE_TAG, UdaAttribute(), soundChunk);
            spatialSound->setInaudibleDistance(inaudibleDistanceChunk->getFloatValue());

            return spatialSound;
        } else if (soundType == GLOBAL_VALUE) {
            return std::make_unique<GlobalSound>(filename, category);
        } else {
            throw std::invalid_argument("Unknown sound type read from map: " + soundType);
        }
    }

    void SoundReaderWriter::buildChunkFrom(UdaChunk& soundChunk, const Sound& sound, UdaWriter& udaWriter) {
        auto& filenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &soundChunk);
        filenameChunk.setStringValue(sound.getFilename());

        if (sound.getSoundType() == Sound::SPATIAL) {
            const auto& spatialSound = dynamic_cast<const SpatialSound&>(sound);
            soundChunk.addAttribute(UdaAttribute(TYPE_ATTR, SPATIAL_VALUE));

            auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &soundChunk);
            positionChunk.setPoint3Value(spatialSound.getPosition());

            auto& inaudibleDistanceChunk = udaWriter.createChunk(INAUDIBLE_DISTANCE_TAG, UdaAttribute(), &soundChunk);
            inaudibleDistanceChunk.setFloatValue(spatialSound.getInaudibleDistance());
        } else if (sound.getSoundType() == Sound::GLOBAL) {
            soundChunk.addAttribute(UdaAttribute(TYPE_ATTR, GLOBAL_VALUE));
        } else {
            throw std::invalid_argument("Unknown sound type to write in map: " + std::to_string(sound.getSoundType()));
        }

        if (sound.getSoundCategory() == Sound::SoundCategory::MUSIC) {
            soundChunk.addAttribute(UdaAttribute(CATEGORY_ATTR, MUSIC_VALUE));
        } else if (sound.getSoundCategory() == Sound::SoundCategory::EFFECTS) {
            soundChunk.addAttribute(UdaAttribute(CATEGORY_ATTR, EFFECTS_VALUE));
        } else {
            throw std::invalid_argument("Unknown sound category to write in map: " + std::to_string(sound.getSoundCategory()));
        }
    }

    void SoundReaderWriter::loadPropertiesOn(Sound& sound, const UdaChunk* soundChunk, const UdaParser& udaParser) {
        auto initialVolumeChunk = udaParser.getUniqueChunk(true, INITIAL_VOLUME_TAG, UdaAttribute(), soundChunk);
        sound.setInitialVolume(initialVolumeChunk->getFloatValue());
    }

    void SoundReaderWriter::writePropertiesOn(UdaChunk& soundChunk, const Sound& sound, UdaWriter& udaWriter) {
        auto& initialVolumeChunk = udaWriter.createChunk(INITIAL_VOLUME_TAG, UdaAttribute(), &soundChunk);
        initialVolumeChunk.setFloatValue(sound.getInitialVolume());
    }

}
