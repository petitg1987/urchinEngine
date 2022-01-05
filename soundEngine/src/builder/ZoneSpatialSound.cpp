#include <builder/ZoneSpatialSound.h>

namespace urchin {

    ZoneSpatialSound::ZoneSpatialSound(SoundEnvironment& soundEnvironment, std::shared_ptr<SpatialSound> sound, std::shared_ptr<ZoneTrigger> trigger) :
            soundEnvironment(soundEnvironment),
            sound(std::move(sound)),
            trigger(std::move(trigger)) {
        soundEnvironment.addSound(this->sound, this->trigger);
    }

    ZoneSpatialSound::~ZoneSpatialSound() {
        soundEnvironment.removeSound(*sound);
    }

    SpatialSound& ZoneSpatialSound::getSound() const {
        return *sound;
    }

    ZoneTrigger& ZoneSpatialSound::getTrigger() const {
        return *trigger;
    }

}
