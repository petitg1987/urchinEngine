#include "AudioController.h"
#include "player/stream/AudioStreamPlayer.h"

namespace urchin {

    /**
     * @param soundTrigger Trigger used to play the sound
     */
    AudioController::AudioController(Sound* sound, SoundTrigger* soundTrigger, StreamUpdateWorker* streamUpdateWorker) :
            sound(sound),
            soundTrigger(soundTrigger),
            triggerValue(SoundTrigger::STOP),
            isPaused(false) {
        smoothStopAction = new SmoothStopAction(soundTrigger->getSoundBehavior());

        audioPlayer = new AudioStreamPlayer(sound, streamUpdateWorker);
    }

    AudioController::~AudioController() {
        audioPlayer->stop();
        delete audioPlayer;

        delete smoothStopAction;

        delete sound;
        delete soundTrigger;
    }

    const Sound *AudioController::getSound() const {
        return sound;
    }

    SoundTrigger *AudioController::getSoundTrigger() const {
        return soundTrigger;
    }

    void AudioController::changeSoundTrigger(SoundTrigger* newSoundTrigger) {
        audioPlayer->stop();
        triggerValue = SoundTrigger::STOP;

        delete smoothStopAction;
        delete soundTrigger;

        soundTrigger = newSoundTrigger;
        smoothStopAction = new SmoothStopAction(soundTrigger->getSoundBehavior());
    }

    void AudioController::pause() {
        if (audioPlayer->isPlaying()) {
            audioPlayer->pause();

            isPaused = true;
        }
    }

    void AudioController::unpause() {
        if (isPaused) {
            audioPlayer->play(); //as it's a unpause: use 'play' or 'playLoop' method doesn't make any difference

            isPaused = false;
        }
    }

    void AudioController::process(const Point3<float>& listenerPosition) {
        if (triggerValue!=SoundTrigger::STOP && triggerValue!=SoundTrigger::SMOOTH_STOP
                && sound->isStopped()) {
            triggerValue = SoundTrigger::STOP;
        }

        SoundTrigger::TriggerResultValue oldTriggerValue = triggerValue;
        SoundTrigger::TriggerResultValue newTriggerValue = soundTrigger->evaluateTrigger(listenerPosition);
        if (newTriggerValue!=SoundTrigger::NO_TRIGGER) {
            triggerValue = newTriggerValue;
        }

        processTriggerValue(oldTriggerValue);
    }

    void AudioController::processTriggerValue(SoundTrigger::TriggerResultValue oldTriggerValue) {
        processSmoothStopTriggerValue(oldTriggerValue);

        if (triggerValue!=oldTriggerValue) {
            if (triggerValue==SoundTrigger::PLAY) {
                audioPlayer->play();
            } else if (triggerValue==SoundTrigger::PLAY_LOOP) {
                audioPlayer->playLoop();
            } else if (triggerValue==SoundTrigger::STOP) {
                audioPlayer->stop();
            } else if (triggerValue==SoundTrigger::PAUSE) {
                audioPlayer->pause();
            }
        }
    }

    void AudioController::processSmoothStopTriggerValue(SoundTrigger::TriggerResultValue oldTriggerValue) {
        //process smooth stop if started
        if (triggerValue==SoundTrigger::SMOOTH_STOP && smoothStopAction->isSmoothStopStarted()) {
            if (smoothStopAction->isSmoothStopProcessing()) {
                sound->setVolumeChange(smoothStopAction->computeChangeVolumePercentage());
            } else {
                triggerValue = SoundTrigger::STOP;
            }
        }

        //check smooth stop state
        if (triggerValue!=oldTriggerValue) {
            if (oldTriggerValue==SoundTrigger::SMOOTH_STOP) {
                //reset original volume before smooth stop
                sound->setVolumeChange(-smoothStopAction->getTotalChangeVolumePercentage());

                smoothStopAction->endSmoothStop();
            } else if (triggerValue==SoundTrigger::SMOOTH_STOP) {
                smoothStopAction->startSmoothStop();
            }
        }
    }

}
