#include <AL/al.h>
#include <algorithm>
#include <stdexcept>

#include "SoundManager.h"

namespace urchin {

    SoundManager::SoundManager() {
        DeviceManager::instance();
        streamUpdateWorker = new StreamUpdateWorker();
        streamUpdateWorkerThread = new std::thread(&StreamUpdateWorker::start, streamUpdateWorker);

        alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
        alListener3f(AL_POSITION, 0.f, 0.f, 0.f);
    }

    SoundManager::~SoundManager() {
        for (auto* audioController : audioControllers) {
            deleteAudioController(audioController);
        }

        streamUpdateWorker->interrupt();
        streamUpdateWorkerThread->join();
        delete streamUpdateWorkerThread;
        delete streamUpdateWorker;

        Profiler::getInstance("sound")->log();
    }

    void SoundManager::addSound(Sound* sound, SoundTrigger* soundTrigger) {
        if (sound && soundTrigger) {
            auto* audioController = new AudioController(sound, soundTrigger, streamUpdateWorker);
            audioControllers.push_back(audioController);
        }
    }

    void SoundManager::removeSound(const Sound* sound) {
        for (auto it = audioControllers.begin(); it!=audioControllers.end(); ++it) {
            if ((*it)->getSound() == sound) {
                deleteAudioController(*it);
                audioControllers.erase(it);

                break;
            }
        }
    }

    void SoundManager::changeSoundTrigger(const Sound* sound, SoundTrigger* newSoundTrigger) {
        for (auto& audioController : audioControllers) {
            if (audioController->getSound() == sound) {
                audioController->changeSoundTrigger(newSoundTrigger);

                break;
            }
        }
    }

    std::vector<const SoundTrigger*> SoundManager::getSoundTriggers() const {
        std::vector<const SoundTrigger*> triggers;

        for (const auto& audioController : audioControllers) {
            triggers.push_back(audioController->getSoundTrigger());
        }

        return triggers;
    }

    SoundTrigger* SoundManager::retrieveSoundTriggerFor(const Sound* sound) const {
        for (const auto& audioController : audioControllers) {
            if (audioController->getSound() == sound) {
                return audioController->getSoundTrigger();
            }
        }

        throw std::invalid_argument("Impossible to find a sound trigger for the sound.");
    }

    void SoundManager::pause() {
        for (auto& audioController : audioControllers) {
            audioController->pause();
        }
    }

    void SoundManager::unpause() {
        for (auto& audioController : audioControllers) {
            audioController->unpause();
        }
    }

    void SoundManager::controlExecution() {
        streamUpdateWorker->controlExecution();
    }

    void SoundManager::process(const Point3<float>& listenerPosition) {
        alListener3f(AL_POSITION, listenerPosition.X, listenerPosition.Y, listenerPosition.Z);

        for (auto& audioController : audioControllers) {
            audioController->process(listenerPosition);
        }

        ALenum err;
        while ((err = alGetError()) != AL_NO_ERROR) {
            Logger::logger().logError("OpenAL error detected: " + std::to_string(err));
        }
    }

    void SoundManager::process() {
        ScopeProfiler profiler("sound", "soundMgrProc");

        process(Point3<float>(0.0, 0.0, 0.0));
    }

    void SoundManager::deleteAudioController(AudioController* audioController) {
        delete audioController;
    }
}
