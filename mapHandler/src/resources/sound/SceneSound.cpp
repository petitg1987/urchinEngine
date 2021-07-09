#include <stdexcept>

#include "SceneSound.h"
#include <resources/sound/SoundReaderWriter.h>
#include <resources/sound/SoundTriggerReaderWriter.h>

namespace urchin {

    SceneSound::SceneSound() :
            soundManager(nullptr),
            sound(nullptr),
            soundTrigger(nullptr) {

    }

    SceneSound::~SceneSound() {
        if (soundManager) {
            soundManager->removeSound(sound);
        } else {
            delete this->sound;
            delete this->soundTrigger;
        }
    }

    void SceneSound::setSoundManager(SoundManager* soundManager) {
        this->soundManager = soundManager;

        if (soundManager) {
            soundManager->addSound(sound, soundTrigger);
        }
    }

    void SceneSound::loadFrom(const DataChunk* chunk, const DataParser& dataParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        auto soundChunk = dataParser.getUniqueChunk(true, SOUND_TAG, DataAttribute(), chunk);
        auto soundTriggerChunk = dataParser.getUniqueChunk(true, SOUND_TRIGGER_TAG, DataAttribute(), chunk);

        setSoundElements(SoundReaderWriter::loadFrom(soundChunk.get(), dataParser), SoundTriggerReaderWriter::loadFrom(soundTriggerChunk.get(), dataParser));
    }

    void SceneSound::writeOn(DataChunk* chunk, DataWriter& dataWriter) const {
        chunk->setAttribute(DataAttribute(NAME_ATTR, this->name));

        auto soundChunk = dataWriter.createChunk(SOUND_TAG, DataAttribute(), chunk);
        auto soundTriggerChunk = dataWriter.createChunk(SOUND_TRIGGER_TAG, DataAttribute(), chunk);

        SoundReaderWriter::writeOn(soundChunk.get(), sound, dataWriter);
        SoundTriggerReaderWriter::writeOn(soundTriggerChunk.get(), soundTrigger, dataWriter);
    }

    std::string SceneSound::getName() const {
        return name;
    }

    void SceneSound::setName(const std::string& name) {
        this->name = name;
    }

    Sound* SceneSound::getSound() const {
        return sound;
    }

    SoundTrigger* SceneSound::getSoundTrigger() const {
        return soundTrigger;
    }

    void SceneSound::setSoundElements(Sound* sound, SoundTrigger* soundTrigger) {
        if (!sound) {
            throw std::invalid_argument("Cannot set a null sound on scene sound.");
        } else if (!soundTrigger) {
            throw std::invalid_argument("Cannot set a null sound trigger on scene sound.");
        }

        if (soundManager) {
            soundManager->removeSound(this->sound);
            soundManager->addSound(sound, soundTrigger);
        } else {
            delete this->sound;
            delete this->soundTrigger;
        }

        this->sound = sound;
        this->soundTrigger = soundTrigger;
    }

    void SceneSound::changeSoundTrigger(SoundTrigger* newSoundTrigger) {
        if (!sound) {
            throw std::invalid_argument("Cannot change sound trigger without having a sound on scene sound.");
        }
        if (!soundTrigger) {
            throw std::invalid_argument("Cannot set a null sound trigger on scene sound.");
        }

        if (soundManager) {
            soundManager->changeSoundTrigger(sound, newSoundTrigger);
        } else {
            delete this->soundTrigger;
        }

        this->soundTrigger = newSoundTrigger;
    }
}
