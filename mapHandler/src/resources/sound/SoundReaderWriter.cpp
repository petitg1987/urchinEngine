#include <resources/sound/SoundReaderWriter.h>

namespace urchin {

    std::unique_ptr<Sound> SoundReaderWriter::loadFrom(const UdaChunk* soundChunk, const UdaParser& udaParser) {
        return buildSoundFrom(soundChunk, udaParser);
    }

    void SoundReaderWriter::writeOn(UdaChunk& soundChunk, const Sound& sound, UdaWriter& udaWriter) {
        buildChunkFrom(soundChunk, sound, udaWriter);
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

        float initialVolume = udaParser.getUniqueChunk(true, INITIAL_VOLUME_TAG, UdaAttribute(), soundChunk)->getFloatValue();
        std::string soundType = soundChunk->getAttributeValue(TYPE_ATTR);
        if (soundType == SPATIAL_VALUE) {
            Point3<float> position = udaParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), soundChunk)->getPoint3Value();
            float inaudibleDistance = udaParser.getUniqueChunk(true, INAUDIBLE_DISTANCE_TAG, UdaAttribute(), soundChunk)->getFloatValue();
            auto spatialSound = std::make_unique<SpatialSound>(filename, category, initialVolume, position, inaudibleDistance);

            return spatialSound;
        } else if (soundType == GLOBAL_VALUE) {
            return std::make_unique<GlobalSound>(filename, category, initialVolume);
        } else {
            throw std::invalid_argument("Unknown sound type read from map: " + soundType);
        }
    }

    void SoundReaderWriter::buildChunkFrom(UdaChunk& soundChunk, const Sound& sound, UdaWriter& udaWriter) {
        auto& filenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &soundChunk);
        filenameChunk.setStringValue(sound.getFilename());

        auto& initialVolumeChunk = udaWriter.createChunk(INITIAL_VOLUME_TAG, UdaAttribute(), &soundChunk);
        initialVolumeChunk.setFloatValue(sound.getInitialVolume());

        if (sound.getSoundCategory() == Sound::SoundCategory::MUSIC) {
            soundChunk.addAttribute(UdaAttribute(CATEGORY_ATTR, MUSIC_VALUE));
        } else if (sound.getSoundCategory() == Sound::SoundCategory::EFFECTS) {
            soundChunk.addAttribute(UdaAttribute(CATEGORY_ATTR, EFFECTS_VALUE));
        } else {
            throw std::invalid_argument("Unknown sound category to write in map: " + std::to_string(sound.getSoundCategory()));
        }

        if (sound.getSoundType() == Sound::SPATIAL) {
            const auto& spatialSound = static_cast<const SpatialSound&>(sound);
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
    }

}
