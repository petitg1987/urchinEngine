#include <SoundComponent.h>

namespace urchin {

    SoundComponent::SoundComponent(std::shared_ptr<Sound> sound, std::shared_ptr<SoundTrigger> soundTrigger) :
            sound(std::move(sound)),
            soundTrigger(std::move(soundTrigger)) {

    }

    Sound& SoundComponent::getSound() const {
        return *sound;
    }

    SpatialSound& SoundComponent::getSpatialSound() const {
        if (sound->getSoundType() == Sound::SoundType::SPATIAL) {
            return static_cast<SpatialSound&>(*sound);
        }
        throw std::runtime_error("Following sound is not a spatial sound: " + sound->getFilename());
    }

    SoundTrigger& SoundComponent::getSoundTrigger() const {
        return *soundTrigger;
    }

    ManualTrigger& SoundComponent::getManualTrigger() const {
        if (soundTrigger->getTriggerType() == SoundTrigger::TriggerType::MANUAL_TRIGGER) {
            return static_cast<ManualTrigger&>(*soundTrigger);
        }
        throw std::runtime_error("Following sound is not linked to a manual trigger: " + sound->getFilename());
    }

}