#include <utility>

#include "AudioController.h"
#include "player/stream/AudioStreamPlayer.h"

namespace urchin {

    AudioController::AudioController(std::shared_ptr<SoundComponent> soundComponent, StreamUpdateWorker& streamUpdateWorker) :
            soundComponent(std::move(soundComponent)),
            streamUpdateWorker(streamUpdateWorker) {

    }

    AudioController::~AudioController() {
        for (auto& audioStreamPlayer : audioStreamPlayers) {
            audioStreamPlayer.stop();
        }
    }

    SoundComponent& AudioController::getSoundComponent() const {
        return *soundComponent;
    }

    void AudioController::pauseAll() {
        for (auto& audioStreamPlayer : audioStreamPlayers) {
            if (audioStreamPlayer.isPlaying()) {
                audioStreamPlayer.pause();
            }
        }
    }

    void AudioController::unpauseAll() {
        for (auto& audioStreamPlayer : audioStreamPlayers) {
            if (audioStreamPlayer.isPaused()) {
                audioStreamPlayer.unpause();
            }
        }
    }

    std::size_t AudioController::getPlayersCount() const {
        return audioStreamPlayers.size();
    }

    void AudioController::process(const Point3<float>& listenerPosition, const std::map<Sound::SoundCategory, float>& soundVolumes) {
        //remove unused players
        std::erase_if(audioStreamPlayers, [](const AudioStreamPlayer& stp) { return stp.isStopped(); });

        //trigger actions process
        const std::vector<SoundTrigger::TriggerAction>& triggerActions = soundComponent->getSoundTrigger().evaluateTrigger(listenerPosition);
        for (auto triggerAction : triggerActions) {
            processTriggerValue(triggerAction);
        }

        //update audio players (sound position, volume, etc.)
        for (auto& audioStreamPlayer : audioStreamPlayers) {
            soundComponent->getSound().updateSource(audioStreamPlayer.getSourceId());
            audioStreamPlayer.changeVolume(soundVolumes.at(audioStreamPlayer.getSound().getSoundCategory()));
        }
    }

    void AudioController::processTriggerValue(SoundTrigger::TriggerAction triggerAction) {
        if (triggerAction == SoundTrigger::PLAY_NEW) {
            audioStreamPlayers.emplace_back(soundComponent->getSound(), streamUpdateWorker);
            audioStreamPlayers.back().play();
        } else if (triggerAction == SoundTrigger::PLAY_NEW_LOOP) {
            audioStreamPlayers.emplace_back(soundComponent->getSound(), streamUpdateWorker);
            audioStreamPlayers.back().playLoop();
        } else if (triggerAction == SoundTrigger::STOP_ALL) {
            for (auto& audioStreamPlayer : audioStreamPlayers) {
                audioStreamPlayer.stop();
            }
            audioStreamPlayers.clear();
        } else if (triggerAction == SoundTrigger::PAUSE_ALL) {
            for (auto& audioStreamPlayer : audioStreamPlayers) {
                audioStreamPlayer.pause();
            }
        } else if (triggerAction == SoundTrigger::UNPAUSE_ALL) {
            for (auto& audioStreamPlayer : audioStreamPlayers) {
                audioStreamPlayer.unpause();
            }
        }
    }

}
