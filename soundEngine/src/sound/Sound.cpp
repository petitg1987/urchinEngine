#include <utility>

#include <sound/Sound.h>

namespace urchin {

    /**
     * @param initialVolume Initial sound volume (0.0=minimum volume, 1.0=original volume). Volume can be higher to 1.0.
     */
    Sound::Sound(std::string filename, SoundCategory category, float initialVolume) :
            filename(std::move(filename)),
            category(category),
            initialVolume(initialVolume) {

    }

    const std::string& Sound::getFilename() const {
        return filename;
    }

    Sound::SoundCategory Sound::getSoundCategory() const {
        return category;
    }

    float Sound::getInitialVolume() const {
        return initialVolume;
    }

}
