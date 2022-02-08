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

    void SoundEnvironment::addSoundComponent(std::shared_ptr<SoundComponent> soundComponent) {
        ScopeProfiler sp(Profiler::sound(), "addSoundComponent");

        audioControllers.push_back(std::make_unique<AudioController>(std::move(soundComponent), streamUpdateWorker));
    }

    void SoundEnvironment::removeSoundComponent(const SoundComponent& soundComponent) {
        std::size_t erasedCount = std::erase_if(audioControllers, [&soundComponent](const auto& ac){ return &ac->getSoundComponent() == &soundComponent; });
        if (erasedCount != 1) {
            throw std::runtime_error("Removing the sound component fail: " + soundComponent.getSound().getFilename());
        }
    }

    const AudioController& SoundEnvironment::getAudioController(const SoundComponent& soundComponent) const {
        auto findAudioController = std::ranges::find_if(audioControllers, [&soundComponent](const auto& ac) { return &ac->getSoundComponent() == &soundComponent; });
        if (findAudioController == audioControllers.end()) {
            throw std::runtime_error("Retrieve the sound component fail: " + soundComponent.getSound().getFilename());
        }
        return *(*findAudioController);
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
