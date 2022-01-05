#include <builder/ZoneSound.h>

namespace urchin {

    ZoneSound::ZoneSound(SoundEnvironment& soundEnvironment, std::shared_ptr<GlobalSound> sound, std::shared_ptr<ShapeTrigger> trigger) :
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

    ShapeTrigger& ZoneSound::getTrigger() const {
        return *trigger;
    }

}
