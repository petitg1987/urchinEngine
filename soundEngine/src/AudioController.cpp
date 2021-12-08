#include <AudioController.h>
#include <player/stream/AudioStreamPlayer.h>

namespace urchin {

    /**
     * @param soundTrigger Trigger used to play the sound
     */
    AudioController::AudioController(std::shared_ptr<Sound> sound, std::shared_ptr<SoundTrigger> soundTrigger, StreamUpdateWorker& streamUpdateWorker) :
            sound(std::move(sound)),
            soundTrigger(std::move(soundTrigger)),
            streamUpdateWorker(streamUpdateWorker) {

    }

    AudioController::~AudioController() {
        for (const auto& audioPlayer : audioPlayers) {
            audioPlayer->stop();
        }
    }

    Sound& AudioController::getSound() const {
        return *sound;
    }

    SoundTrigger& AudioController::getSoundTrigger() const {
        return *soundTrigger;
    }

    void AudioController::changeSoundTrigger(std::shared_ptr<SoundTrigger> newSoundTrigger) {
        for (const auto& audioPlayer : audioPlayers) {
            audioPlayer->stop();
        }
        audioPlayers.clear();

        soundTrigger = std::move(newSoundTrigger);
    }

    void AudioController::pauseAll() {
        for (const auto& audioPlayer : audioPlayers) {
            if (audioPlayer->isPlaying()) {
                audioPlayer->pause();
            }
        }
    }

    void AudioController::unpauseAll() {
        for (const auto& audioPlayer : audioPlayers) {
            if (audioPlayer->isPaused()) {
                audioPlayer->unpause();
            }
        }
    }

    void AudioController::process(const Point3<float>& listenerPosition, const std::map<Sound::SoundCategory, float>& soundVolumes) {
        //remove unused players
        for (auto it = audioPlayers.begin(); it != audioPlayers.end();) {
            if ((*it)->isStopped()) {
                it = audioPlayers.erase(it);
            } else {
                ++it;
            }
        }

        //trigger actions process
        const std::vector<SoundTrigger::TriggerAction>& triggerActions = soundTrigger->evaluateTrigger(listenerPosition);
        for (auto triggerAction : triggerActions) {
            processTriggerValue(triggerAction);
        }

        //update audio players (sound position, volume, etc.)
        for (const auto& audioPlayer : audioPlayers) {
            sound->updateSource(audioPlayer->getSourceId());
            audioPlayer->changeVolume(soundVolumes.at(audioPlayer->getSound().getSoundCategory()));
        }
    }

    void AudioController::processTriggerValue(SoundTrigger::TriggerAction triggerAction) {
        if (triggerAction == SoundTrigger::PLAY_NEW) {
            audioPlayers.push_back(std::make_unique<AudioStreamPlayer>(*this->sound, streamUpdateWorker));
            audioPlayers.back()->play();
        } else if (triggerAction == SoundTrigger::PLAY_NEW_LOOP) {
            audioPlayers.push_back(std::make_unique<AudioStreamPlayer>(*this->sound, streamUpdateWorker));
            audioPlayers.back()->playLoop();
        } else if (triggerAction == SoundTrigger::STOP_ALL) {
            for (const auto& audioPlayer : audioPlayers) {
                audioPlayer->stop();
            }
            audioPlayers.clear();
        } else if (triggerAction == SoundTrigger::PAUSE_ALL) {
            for (const auto& audioPlayer : audioPlayers) {
                audioPlayer->pause();
            }
        } else if (triggerAction == SoundTrigger::UNPAUSE_ALL) {
            for (const auto& audioPlayer : audioPlayers) {
                audioPlayer->unpause();
            }
        }
    }

}
