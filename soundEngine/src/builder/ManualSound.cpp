#include <builder/ManualSound.h>

namespace urchin {

    ManualSound::ManualSound(SoundEnvironment& soundEnvironment, std::shared_ptr<GlobalSound> sound, std::shared_ptr<ManualTrigger> trigger) :
            soundEnvironment(soundEnvironment),
            sound(std::move(sound)),
            trigger(std::move(trigger)) {
        soundEnvironment.addSound(this->sound, this->trigger);
    }

    ManualSound::~ManualSound() {
        soundEnvironment.removeSound(*sound);
    }

    GlobalSound& ManualSound::getSound() const {
        return *sound;
    }

    ManualTrigger& ManualSound::getTrigger() const {
        return *trigger;
    }

}
