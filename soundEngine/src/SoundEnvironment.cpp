#include <AL/al.h>
#include <stdexcept>

#include <SoundEnvironment.h>

namespace urchin {

    SoundEnvironment::SoundEnvironment() :
            streamUpdateWorker(StreamUpdateWorker()),
            streamUpdateWorkerThread(std::jthread(&StreamUpdateWorker::start, &streamUpdateWorker)) {
        SignalHandler::instance().initialize();

        AudioDevice::instance().enable(true);
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);

        soundVolumes[Sound::SoundCategory::MUSIC] = 1.0f;
        soundVolumes[Sound::SoundCategory::EFFECTS] = 1.0f;
    }

    SoundEnvironment::~SoundEnvironment() {
        audioControllers.clear();

        streamUpdateWorker.interrupt();
        streamUpdateWorkerThread.join();

        Profiler::sound().log();
    }

    void SoundEnvironment::addSound(std::shared_ptr<Sound> sound, std::shared_ptr<SoundTrigger> soundTrigger) {
        if (sound && soundTrigger) {
            ScopeProfiler sp(Profiler::sound(), "addSound");

            audioControllers.push_back(std::make_unique<AudioController>(std::move(sound), std::move(soundTrigger), streamUpdateWorker));
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
        for (const auto& audioController : audioControllers) {
            if (&audioController->getSound() == &sound) {
                audioController->changeSoundTrigger(std::move(newSoundTrigger));
                break;
            }
        }
    }

    void SoundEnvironment::addSoundComponent(std::shared_ptr<SoundComponent> soundComponent) {
        ScopeProfiler sp(Profiler::sound(), "addSoundComponent");

        audioControllers.push_back(std::make_unique<AudioController>(std::move(soundComponent), streamUpdateWorker));
    }

    void SoundEnvironment::removeSoundComponent(const SoundComponent& soundComponent) {
        std::size_t erasedCount = std::erase_if(audioControllers, [&soundComponent](const std::unique_ptr<AudioController>& ac){ return &ac->getSound() == &soundComponent.getSound(); });
        if (erasedCount != 1) {
            throw std::runtime_error("Removing the sound component fail: " + soundComponent.getSound().getFilename());
        }
    }

    void SoundEnvironment::setupSoundsVolume(Sound::SoundCategory soundCategory, float volumePercentageChange) {
        soundVolumes[soundCategory] = volumePercentageChange;
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
        for (const auto& audioController : audioControllers) {
            audioController->pauseAll();
        }
    }

    void SoundEnvironment::unpause() {
        for (const auto& audioController : audioControllers) {
            audioController->unpauseAll();
        }
    }

    /**
     * Check if thread has been stopped by an exception and rethrow exception on main thread
     */
    void SoundEnvironment::checkNoExceptionRaised() {
        streamUpdateWorker.checkNoExceptionRaised();
    }

    void SoundEnvironment::process(const Point3<float>& listenerPosition, const Vector3<float>& listenerFrontVector, const Vector3<float>& listenerUpVector) {
        ScopeProfiler sp(Profiler::sound(), "soundMgrProc");

        alListener3f(AL_POSITION, listenerPosition.X, listenerPosition.Y, listenerPosition.Z);
        struct {
            Vector3<float> frontVector;
            Vector3<float> upVector;
        } listenerOrientation = {listenerFrontVector, listenerUpVector};
        alListenerfv(AL_ORIENTATION, &listenerOrientation.frontVector.X);
        alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);

        for (const auto& audioController : audioControllers) {
            audioController->process(listenerPosition, soundVolumes);
        }

        ALenum err;
        while ((err = alGetError()) != AL_NO_ERROR) {
            Logger::instance().logError("OpenAL error detected: " + std::to_string(err));
        }
    }

    void SoundEnvironment::process() {
        process(Point3<float>(0.0f, 0.0f, 0.0f), Vector3<float>(0.0f, 0.0f, 0.0f), Vector3<float>(0.0f, 1.0f, 0.0f));
    }

}
