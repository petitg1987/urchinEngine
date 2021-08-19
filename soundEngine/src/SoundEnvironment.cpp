#include <AL/al.h>
#include <algorithm>
#include <stdexcept>

#include <SoundEnvironment.h>

namespace urchin {

    SoundEnvironment::SoundEnvironment() :
            streamUpdateWorker(std::make_unique<StreamUpdateWorker>()),
            streamUpdateWorkerThread(std::make_unique<std::thread>(&StreamUpdateWorker::start, streamUpdateWorker.get())) {
        SignalHandler::instance().initialize();

        AudioDevice::instance().enable(true);
        alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    }

    SoundEnvironment::~SoundEnvironment() {
        audioControllers.clear();

        streamUpdateWorker->interrupt();
        streamUpdateWorkerThread->join();

        Profiler::sound().log();
    }

    void SoundEnvironment::addSound(std::shared_ptr<Sound> sound, std::shared_ptr<SoundTrigger> soundTrigger) {
        if (sound && soundTrigger) {
            Logger::instance().logInfo("Add sound: " + sound->getFilename());

            auto audioController = std::make_unique<AudioController>(std::move(sound), std::move(soundTrigger), *streamUpdateWorker);
            adjustSoundVolume(audioController->getAudioPlayer());
            audioControllers.push_back(std::move(audioController));
        }
    }

    void SoundEnvironment::removeSound(const Sound& sound) {
        for (auto it = audioControllers.begin(); it != audioControllers.end(); ++it) {
            if (&(*it)->getSound() == &sound) {
                audioControllers.erase(it);
                break;
            }
        }
    }

    void SoundEnvironment::changeSoundTrigger(const Sound& sound, std::shared_ptr<SoundTrigger> newSoundTrigger) {
        for (auto& audioController : audioControllers) {
            if (&audioController->getSound() == &sound) {
                audioController->changeSoundTrigger(std::move(newSoundTrigger));
                break;
            }
        }
    }

    void SoundEnvironment::setupSoundsVolume(Sound::SoundCategory soundCategory, float volumePercentageChange) {
        soundVolumes[soundCategory] = volumePercentageChange;

        //apply volume on existing sounds:
        for (auto& audioController : audioControllers) {
            if (audioController->getSound().getSoundCategory() == soundCategory) {
                audioController->getAudioPlayer().changeVolume(volumePercentageChange);
            }
        }
    }

    void SoundEnvironment::adjustSoundVolume(AudioPlayer& audioPlayer) {
        auto itVolume = soundVolumes.find(audioPlayer.getSound().getSoundCategory());
        if (itVolume != soundVolumes.end()) {
            audioPlayer.changeVolume(itVolume->second);
        }
    }

    /**
     * @param masterVolume to set (0.0 for minimum volume, 1.0 for original volume). Note that volume can be higher to 1.0.
     */
    void SoundEnvironment::setMasterVolume(float masterVolume) {
        alListenerf(AL_GAIN, masterVolume);
    }

    float SoundEnvironment::getMasterVolume() const {
        float masterVolume = 0.0f;
        alGetListenerf(AL_GAIN, &masterVolume);
        return masterVolume;
    }

    void SoundEnvironment::pause() {
        for (auto& audioController : audioControllers) {
            audioController->pause();
        }
    }

    void SoundEnvironment::unpause() {
        for (auto& audioController : audioControllers) {
            audioController->unpause();
        }
    }

    /**
     * Check if thread has been stopped by an exception and rethrow exception on main thread
     */
    void SoundEnvironment::checkNoExceptionRaised() {
        streamUpdateWorker->checkNoExceptionRaised();
    }

    void SoundEnvironment::process(const Point3<float>& listenerPosition) {
        ScopeProfiler sp(Profiler::sound(), "soundMgrProc");

        alListener3f(AL_POSITION, listenerPosition.X, listenerPosition.Y, listenerPosition.Z);

        for (auto& audioController : audioControllers) {
            audioController->process(listenerPosition);
        }

        ALenum err;
        while ((err = alGetError()) != AL_NO_ERROR) {
            Logger::instance().logError("OpenAL error detected: " + std::to_string(err));
        }
    }

    void SoundEnvironment::process() {
        process(Point3<float>(0.0f, 0.0f, 0.0f));
    }

}
