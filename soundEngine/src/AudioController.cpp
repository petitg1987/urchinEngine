#include <AudioController.h>
#include <player/stream/AudioStreamPlayer.h>

#include <utility>

namespace urchin {

    AudioController::AudioController(std::shared_ptr<SoundComponent> soundComponent, StreamUpdateWorker& streamUpdateWorker) :
            soundComponent(std::move(soundComponent)),
            streamUpdateWorker(streamUpdateWorker) {

    }

    AudioController::~AudioController() {
        for (const auto& audioPlayer : audioPlayers) {
            audioPlayer->stop();
        }
    }

    SoundComponent& AudioController::getSoundComponent() const {
        return *soundComponent;
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

    std::size_t AudioController::getPlayersCount() const {
        return audioPlayers.size();
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
        const std::vector<SoundTrigger::TriggerAction>& triggerActions = soundComponent->getSoundTrigger().evaluateTrigger(listenerPosition);
        for (auto triggerAction : triggerActions) {
            processTriggerValue(triggerAction);
        }

        //update audio players (sound position, volume, etc.)
        for (const auto& audioPlayer : audioPlayers) {
            soundComponent->getSound().updateSource(audioPlayer->getSourceId());
            audioPlayer->changeVolume(soundVolumes.at(audioPlayer->getSound().getSoundCategory()));
        }
    }

    void AudioController::processTriggerValue(SoundTrigger::TriggerAction triggerAction) {
        if (triggerAction == SoundTrigger::PLAY_NEW) {
            audioPlayers.push_back(std::make_unique<AudioStreamPlayer>(soundComponent->getSound(), streamUpdateWorker));
            audioPlayers.back()->play();
        } else if (triggerAction == SoundTrigger::PLAY_NEW_LOOP) {
            audioPlayers.push_back(std::make_unique<AudioStreamPlayer>(soundComponent->getSound(), streamUpdateWorker));
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
