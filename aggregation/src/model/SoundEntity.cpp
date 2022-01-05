#include <stdexcept>

#include <model/SoundEntity.h>

namespace urchin {

    SoundEntity::SoundEntity() :
            soundEnvironment(nullptr) {

    }

    SoundEntity::~SoundEntity() {
        if (soundEnvironment) {
            soundEnvironment->removeSoundComponent(*soundComponent);
        }
    }

    void SoundEntity::setup(SoundEnvironment& soundEnvironment) {
        this->soundEnvironment = &soundEnvironment;

        soundEnvironment.addSoundComponent(soundComponent);
    }

    std::string SoundEntity::getName() const {
        return name;
    }

    void SoundEntity::setName(const std::string& name) {
        this->name = name;
    }

    SoundComponent* SoundEntity::getSoundComponent() const {
        return soundComponent.get();
    }

    void SoundEntity::setSoundComponent(const std::shared_ptr<SoundComponent>& soundComponent) {
        if (soundEnvironment) {
            soundEnvironment->removeSoundComponent(*this->soundComponent);
            soundEnvironment->addSoundComponent(soundComponent);
        }

        this->soundComponent = soundComponent;
    }

}
