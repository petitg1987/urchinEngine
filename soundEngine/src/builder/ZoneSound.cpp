#include <builder/ZoneSound.h>

namespace urchin {

    ZoneSound::ZoneSound(SoundEnvironment& soundEnvironment, std::shared_ptr<GlobalSound> sound, std::shared_ptr<ZoneTrigger> trigger) :
            soundEnvironment(soundEnvironment),
            sound(std::move(sound)),
            trigger(std::move(trigger)) {
        soundEnvironment.addSound(this->sound, this->trigger);
    }

    ZoneSound::~ZoneSound() {
        soundEnvironment.removeSound(*sound);
    }

    GlobalSound& ZoneSound::getSound() const {
        return *sound;
    }

    ZoneTrigger& ZoneSound::getTrigger() const {
        return *trigger;
    }

}
