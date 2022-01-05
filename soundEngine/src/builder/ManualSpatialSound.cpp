#include <builder/ManualSpatialSound.h>

namespace urchin {

    ManualSpatialSound::ManualSpatialSound(SoundEnvironment& soundEnvironment, std::shared_ptr<SpatialSound> sound, std::shared_ptr<ManualTrigger> trigger) :
            soundEnvironment(soundEnvironment),
            sound(std::move(sound)),
            trigger(std::move(trigger)) {
        soundEnvironment.addSound(this->sound, this->trigger);
    }

    ManualSpatialSound::~ManualSpatialSound() {
        soundEnvironment.removeSound(*sound);
    }

    SpatialSound& ManualSpatialSound::getSound() const {
        return *sound;
    }

    ManualTrigger& ManualSpatialSound::getTrigger() const {
        return *trigger;
    }

}
