#include "SoundComponent.h"

namespace urchin {

    SoundComponent::SoundComponent(std::shared_ptr<Sound> sound, std::shared_ptr<SoundTrigger> soundTrigger) :
            sound(std::move(sound)),
            soundTrigger(std::move(soundTrigger)) {

    }

    SoundComponent::SoundComponent(const SoundComponent& soundComponent) :
            sound(soundComponent.getSound().clone()),
            soundTrigger(soundComponent.getSoundTrigger().clone(sound)) {

    }

    Sound& SoundComponent::getSound() const {
        return *sound;
    }

    GlobalSound& SoundComponent::getGlobalSound() const {
        if (sound->getSoundType() == Sound::SoundType::GLOBAL) {
            return static_cast<GlobalSound&>(*sound);
        }
        throw std::runtime_error("Following sound is not a global sound: " + sound->getFilename());
    }

    LocalizableSound& SoundComponent::getLocalizableSound() const {
        if (sound->getSoundType() == Sound::SoundType::LOCALIZABLE) {
            return static_cast<LocalizableSound&>(*sound);
        }
        throw std::runtime_error("Following sound is not a localizable sound: " + sound->getFilename());
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

    AreaTrigger& SoundComponent::getAreaTrigger() const {
        if (soundTrigger->getTriggerType() == SoundTrigger::TriggerType::AREA_TRIGGER) {
            return static_cast<AreaTrigger&>(*soundTrigger);
        }
        throw std::runtime_error("Following sound is not linked to an area trigger: " + sound->getFilename());
    }

}