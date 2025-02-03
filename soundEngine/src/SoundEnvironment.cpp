#include <AL/al.h>
#include <stdexcept>

#include <SoundEnvironment.h>
#include <device/AudioDevice.h>
#include <util/CheckState.h>

namespace urchin {

    SoundEnvironment::SoundEnvironment() :
            soundBuilder(SoundBuilder(*this)),
            streamUpdateWorker(StreamUpdateWorker()),
            streamUpdateWorkerThread(std::jthread(&StreamUpdateWorker::start, &streamUpdateWorker)) {
        SignalHandler::instance().initialize();

        AudioDevice::instance().enable();
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        CheckState::check("set listener position to origin");

        soundVolumes[Sound::SoundCategory::MUSIC] = 1.0f;
        soundVolumes[Sound::SoundCategory::EFFECTS] = 1.0f;
    }

    SoundEnvironment::~SoundEnvironment() {
        musicLoopPlayers.clear();
        audioControllers.clear();

        streamUpdateWorker.interruptThread();
        streamUpdateWorkerThread.join();

        Profiler::sound().log();
    }

    SoundBuilder& SoundEnvironment::getBuilder() {
        return soundBuilder;
    }

    void SoundEnvironment::addSoundComponent(std::shared_ptr<SoundComponent> soundComponent) {
        ScopeProfiler sp(Profiler::sound(), "addSoundComp");

        audioControllers.push_back(std::make_unique<AudioController>(std::move(soundComponent), streamUpdateWorker));
    }

    void SoundEnvironment::removeSoundComponent(const SoundComponent* soundComponent) {
        if (soundComponent) {
            std::size_t erasedCount = std::erase_if(audioControllers, [&soundComponent](const auto& ac){ return &ac->getSoundComponent() == soundComponent; });
            if (erasedCount != 1) {
                throw std::runtime_error("Removing the sound component fail: " + soundComponent->getSound().getFilename());
            }
        }
    }

    const AudioController& SoundEnvironment::getAudioController(const SoundComponent& soundComponent) const {
        auto findAudioController = std::ranges::find_if(audioControllers, [&soundComponent](const auto& ac) { return &ac->getSoundComponent() == &soundComponent; });
        if (findAudioController == audioControllers.end()) {
            throw std::runtime_error("Retrieve the sound component fail: " + soundComponent.getSound().getFilename());
        }
        return *(*findAudioController);
    }

    void SoundEnvironment::addMusicLoopPlayer(std::shared_ptr<MusicLoopPlayer> musicLoopPlayer) {
        musicLoopPlayer->setup(*this);
        musicLoopPlayers.push_back(std::move(musicLoopPlayer));
    }

    void SoundEnvironment::removeMusicLoopPlayer(const MusicLoopPlayer& musicLoopPlayer) {
        std::size_t erasedCount = std::erase_if(musicLoopPlayers, [&musicLoopPlayer](const auto& mlp){ return mlp.get() == &musicLoopPlayer; });
        if (erasedCount != 1) {
            throw std::runtime_error("Removing the music loop player fail");
        }
    }

    void SoundEnvironment::setupSoundsVolume(Sound::SoundCategory soundCategory, float volumePercentageChange) {
        soundVolumes[soundCategory] = volumePercentageChange;
    }

    /**
     * @param masterVolume Master volume (0.0=minimum volume, 1.0=original volume)
     */
    void SoundEnvironment::setMasterVolume(float masterVolume) const {
        if (masterVolume < 0.0f || masterVolume > 1.0f) {
            throw std::runtime_error("Master volume is outside the acceptable range: " + std::to_string(masterVolume));
        }
        alListenerf(AL_GAIN, masterVolume);
        CheckState::check("set listener gain", masterVolume);
    }

    float SoundEnvironment::getMasterVolume() const {
        float masterVolume = 0.0f;
        alGetListenerf(AL_GAIN, &masterVolume);
        CheckState::check("get listener gain");
        return masterVolume;
    }

    void SoundEnvironment::pause() const {
        for (const auto& audioController : audioControllers) {
            audioController->pauseAll();
        }
    }

    void SoundEnvironment::unpause() const {
        for (const auto& audioController : audioControllers) {
            audioController->unpauseAll();
        }
    }

    void SoundEnvironment::process(const Point3<float>& listenerPosition, const Vector3<float>& listenerFrontVector, const Vector3<float>& listenerUpVector) const {
        ScopeProfiler sp(Profiler::sound(), "soundMgrProc");

        alListener3f(AL_POSITION, listenerPosition.X, listenerPosition.Y, listenerPosition.Z);
        CheckState::check("set listener position", listenerPosition);

        struct {
            Vector3<float> frontVector;
            Vector3<float> upVector;
        } listenerOrientation = {.frontVector=listenerFrontVector, .upVector=listenerUpVector};
        alListenerfv(AL_ORIENTATION, &listenerOrientation.frontVector.X);
        CheckState::check("set listener orientation", listenerOrientation.frontVector, listenerOrientation.upVector);

        alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        CheckState::check("set listener velocity");

        for (const auto& musicLoopPlayer : musicLoopPlayers) {
            musicLoopPlayer->refresh();
        }
        for (const auto& audioController : audioControllers) {
            audioController->process(listenerPosition, soundVolumes);
        }
    }

    void SoundEnvironment::process() const {
        process(Point3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
    }

}
