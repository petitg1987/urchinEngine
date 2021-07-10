#include <AL/al.h>
#include <algorithm>
#include <stdexcept>

#include <SoundManager.h>

namespace urchin {

    SoundManager::SoundManager() :
            streamUpdateWorker(std::make_unique<StreamUpdateWorker>()),
            streamUpdateWorkerThread(std::make_unique<std::thread>(&StreamUpdateWorker::start, streamUpdateWorker.get())) {
        SignalHandler::instance()->initialize();

        DeviceManager::instance();
        alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    }

    SoundManager::~SoundManager() {
        audioControllers.clear();

        streamUpdateWorker->interrupt();
        streamUpdateWorkerThread->join();

        Profiler::sound()->log();
    }

    void SoundManager::addSound(Sound* sound, SoundTrigger* soundTrigger) {
        if (sound && soundTrigger) {
            Logger::instance()->logInfo("Add sound: " + sound->getFilename());
            adjustSoundVolume(sound);

            auto audioController = std::make_unique<AudioController>(sound, soundTrigger, *streamUpdateWorker);
            audioControllers.push_back(std::move(audioController));
        }
    }

    void SoundManager::removeSound(const Sound* sound) {
        for (auto it = audioControllers.begin(); it != audioControllers.end(); ++it) {
            if ((*it)->getSound() == sound) {
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

    void SoundManager::setupSoundsVolume(Sound::SoundCategory soundCategory, float volumePercentageChange) {
        soundVolumes[soundCategory] = volumePercentageChange;

        //apply volume on existing sounds:
        for (auto& audioController : audioControllers) {
            if (audioController->getSound()->getSoundCategory() == soundCategory) {
                audioController->getSound()->changeVolume(volumePercentageChange);
            }
        }
    }

    void SoundManager::adjustSoundVolume(Sound* sound) {
        auto itVolume = soundVolumes.find(sound->getSoundCategory());
        if (itVolume != soundVolumes.end()) {
            sound->changeVolume(itVolume->second);
        }
    }

    /**
     * @param masterVolume to set (0.0 for minimum volume, 1.0 for original volume). Note that volume can be higher to 1.0.
     */
    void SoundManager::setMasterVolume(float masterVolume) {
        alListenerf(AL_GAIN, masterVolume);
    }

    float SoundManager::getMasterVolume() const {
        float masterVolume = 0.0f;
        alGetListenerf(AL_GAIN, &masterVolume);
        return masterVolume;
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

    /**
     * Check if thread has been stopped by an exception and rethrow exception on main thread
     */
    void SoundManager::checkNoExceptionRaised() {
        streamUpdateWorker->checkNoExceptionRaised();
    }

    void SoundManager::process(const Point3<float>& listenerPosition) {
        ScopeProfiler sp(Profiler::sound(), "soundMgrProc");

        alListener3f(AL_POSITION, listenerPosition.X, listenerPosition.Y, listenerPosition.Z);

        for (auto& audioController : audioControllers) {
            audioController->process(listenerPosition);
        }

        ALenum err;
        while ((err = alGetError()) != AL_NO_ERROR) {
            Logger::instance()->logError("OpenAL error detected: " + std::to_string(err));
        }
    }

    void SoundManager::process() {
        process(Point3<float>(0.0f, 0.0f, 0.0f));
    }

}
