#include <map/save/sound/SoundReaderWriter.h>
#include <util/PathUtil.h>

namespace urchin {

    std::unique_ptr<Sound> SoundReaderWriter::load(const UdaChunk* soundChunk, const UdaParser& udaParser) {
        auto filenameChunk = udaParser.getFirstChunk(true, FILENAME_TAG, UdaAttribute(), soundChunk);
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

        float initialVolume = udaParser.getFirstChunk(true, INITIAL_VOLUME_TAG, UdaAttribute(), soundChunk)->getFloatValue();
        std::string soundType = soundChunk->getAttributeValue(TYPE_ATTR);
        if (soundType == SPATIAL_VALUE) {
            Point3<float> position = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), soundChunk)->getPoint3Value();
            float inaudibleDistance = udaParser.getFirstChunk(true, INAUDIBLE_DISTANCE_TAG, UdaAttribute(), soundChunk)->getFloatValue();
            auto spatialSound = std::make_unique<SpatialSound>(filename, category, initialVolume, position, inaudibleDistance);

            return spatialSound;
        } else if (soundType == GLOBAL_VALUE) {
            return std::make_unique<GlobalSound>(filename, category, initialVolume);
        } else {
            throw std::invalid_argument("Unknown sound type read from map: " + soundType);
        }
    }

    void SoundReaderWriter::write(UdaChunk& soundChunk, const Sound& sound, UdaParser& udaParser) {
        std::string relativeSoundFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), sound.getFilename());
        udaParser.createChunk(FILENAME_TAG, UdaAttribute(), &soundChunk).setStringValue(relativeSoundFilename);

        auto& initialVolumeChunk = udaParser.createChunk(INITIAL_VOLUME_TAG, UdaAttribute(), &soundChunk);
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

            auto& positionChunk = udaParser.createChunk(POSITION_TAG, UdaAttribute(), &soundChunk);
            positionChunk.setPoint3Value(spatialSound.getPosition());

            auto& inaudibleDistanceChunk = udaParser.createChunk(INAUDIBLE_DISTANCE_TAG, UdaAttribute(), &soundChunk);
            inaudibleDistanceChunk.setFloatValue(spatialSound.getInaudibleDistance());
        } else if (sound.getSoundType() == Sound::GLOBAL) {
            soundChunk.addAttribute(UdaAttribute(TYPE_ATTR, GLOBAL_VALUE));
        } else {
            throw std::invalid_argument("Unknown sound type to write in map: " + std::to_string(sound.getSoundType()));
        }
    }

}
