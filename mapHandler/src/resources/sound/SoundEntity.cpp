#include <stdexcept>

#include <resources/sound/SoundEntity.h>
#include <resources/sound/SoundReaderWriter.h>
#include <resources/sound/SoundTriggerReaderWriter.h>

namespace urchin {

    SoundEntity::SoundEntity() :
            soundEnvironment(nullptr) {

    }

    SoundEntity::~SoundEntity() {
        if (soundEnvironment) {
            soundEnvironment->removeSound(*sound);
        }
    }

    void SoundEntity::setup(SoundEnvironment* soundEnvironment) {
        this->soundEnvironment = soundEnvironment;

        if (soundEnvironment) {
            soundEnvironment->addSound(sound, soundTrigger);
        }
    }

    std::string SoundEntity::getName() const {
        return name;
    }

    void SoundEntity::setName(const std::string& name) {
        this->name = name;
    }

    Sound* SoundEntity::getSound() const {
        return sound.get();
    }

    SoundTrigger* SoundEntity::getSoundTrigger() const {
        return soundTrigger.get();
    }

    void SoundEntity::setSoundElements(const std::shared_ptr<Sound>& sound, const std::shared_ptr<SoundTrigger>& soundTrigger) {
        if (!sound) {
            throw std::invalid_argument("Cannot set a null sound on scene sound.");
        } else if (!soundTrigger) {
            throw std::invalid_argument("Cannot set a null sound trigger on scene sound.");
        }

        if (soundEnvironment) {
            soundEnvironment->removeSound(*this->sound);
            soundEnvironment->addSound(sound, soundTrigger);
        }

        this->sound = sound;
        this->soundTrigger = soundTrigger;
    }

    void SoundEntity::changeSoundTrigger(const std::shared_ptr<SoundTrigger>& newSoundTrigger) {
        if (!sound) {
            throw std::invalid_argument("Cannot change sound trigger without having a sound on scene sound.");
        }
        if (!soundTrigger) {
            throw std::invalid_argument("Cannot set a null sound trigger on scene sound.");
        }

        if (soundEnvironment) {
            soundEnvironment->changeSoundTrigger(*sound, newSoundTrigger);
        }

        this->soundTrigger = newSoundTrigger;
    }
}
