template<class SOUND, class TRIGGER> SoundWrapper<SOUND, TRIGGER>::SoundWrapper(SoundEnvironment& soundEnvironment, std::shared_ptr<SOUND> sound, std::shared_ptr<TRIGGER> soundTrigger) :
        soundEnvironment(soundEnvironment),
        sound(std::move(sound)),
        soundTrigger(std::move(soundTrigger)) {
    soundEnvironment.addSound(sound, soundTrigger);
}

template<class SOUND, class TRIGGER> SoundWrapper<SOUND, TRIGGER>::~SoundWrapper() {
    soundEnvironment.removeSound(*sound);
}

template<class SOUND, class TRIGGER> SOUND& SoundWrapper<SOUND, TRIGGER>::getSound() const {
    return *sound;
}

template<class SOUND, class TRIGGER> TRIGGER& SoundWrapper<SOUND, TRIGGER>::getSoundTrigger() const {
    return *soundTrigger;
}
