#include "map/save/object/sound/SoundReaderWriter.h"
#include "util/PathUtil.h"

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
        if (soundType == LOCALIZABLE_VALUE) {
            Point3<float> position = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), soundChunk)->getPoint3Value();
            float radius = udaParser.getFirstChunk(true, RADIUS_TAG, UdaAttribute(), soundChunk)->getFloatValue();
            return std::make_unique<LocalizableSound>(filename, category, initialVolume, position, radius);
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
            throw std::invalid_argument("Unknown sound category to write in map: " + std::to_string((int)sound.getSoundCategory()));
        }

        if (sound.getSoundType() == Sound::SoundType::LOCALIZABLE) {
            const auto& localizableSound = static_cast<const LocalizableSound&>(sound);
            soundChunk.addAttribute(UdaAttribute(TYPE_ATTR, LOCALIZABLE_VALUE));

            auto& positionChunk = udaParser.createChunk(POSITION_TAG, UdaAttribute(), &soundChunk);
            positionChunk.setPoint3Value(localizableSound.getPosition());

            auto& radiusChunk = udaParser.createChunk(RADIUS_TAG, UdaAttribute(), &soundChunk);
            radiusChunk.setFloatValue(localizableSound.getRadius());
        } else if (sound.getSoundType() == Sound::SoundType::GLOBAL) {
            soundChunk.addAttribute(UdaAttribute(TYPE_ATTR, GLOBAL_VALUE));
        } else {
            throw std::invalid_argument("Unknown sound type to write in map: " + std::to_string((int)sound.getSoundType()));
        }
    }

}
