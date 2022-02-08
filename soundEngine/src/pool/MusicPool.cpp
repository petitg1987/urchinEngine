#include <pool/MusicPool.h>
#include <SoundBuilder.h>

namespace urchin {

    MusicPool::MusicPool(SoundEnvironment& soundEnvironment, const std::vector<std::string>& musicFilenames) :
            soundEnvironment(soundEnvironment),
            currentMusicIndex(0),
            isPaused(false) {
        initialize(musicFilenames);
    }

    MusicPool::MusicPool(SoundEnvironment& soundEnvironment, const std::string& musicsDirectory) :
            soundEnvironment(soundEnvironment),
            currentMusicIndex(0),
            isPaused(false) {
        std::vector<std::string> musicFilenames;
        for (const std::string& musicFullPath : FileUtil::getFilesRecursive(FileSystem::instance().getResourcesDirectory() + musicsDirectory)) {
            std::string resourcesMusicFilename = FileUtil::getRelativePath(FileSystem::instance().getResourcesDirectory(), musicFullPath);
            musicFilenames.push_back(resourcesMusicFilename);
        }
        initialize(musicFilenames);
    }

    MusicPool::~MusicPool() {
        for (const auto& music : musics) {
            soundEnvironment.removeSoundComponent(*music.soundComponent);
        }
        musics.clear();
    }

    void MusicPool::initialize(const std::vector<std::string>& musicFilenames) {
        if (musicFilenames.empty()) {
            throw std::runtime_error("At least one music is required");
        }
        for (const std::string& musicFilename : musicFilenames) {
            std::shared_ptr<SoundComponent> soundComponent = SoundBuilder(soundEnvironment).newManualMusic(musicFilename, PlayBehavior::PLAY_ONCE);
            const AudioController& audioController = soundEnvironment.getAudioController(*soundComponent);
            musics.emplace_back(MusicInstance{soundComponent, audioController});
        }
        currentMusicIndex = musics.size() - 1;
    }

    void MusicPool::pause() {
        musics[currentMusicIndex].soundComponent->getManualTrigger().pauseAll();
        isPaused = true;
    }

    void MusicPool::unpause() {
        musics[currentMusicIndex].soundComponent->getManualTrigger().unpauseAll();
        isPaused = false;
    }

    void MusicPool::process() {
        if (!isPaused) {
            if (musics[currentMusicIndex].audioController.getPlayersCount() == 0) {
                currentMusicIndex = (currentMusicIndex + 1) % (unsigned int)musics.size();
                musics[currentMusicIndex].soundComponent->getManualTrigger().playNew();
            }
        }
    }

}
