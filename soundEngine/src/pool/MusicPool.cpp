#include <pool/MusicPool.h>
#include <SoundBuilder.h>
#include <SoundEnvironment.h>

#include <utility>

namespace urchin {

    MusicPool::MusicPool(std::vector<std::string> musicFilenames) :
            soundEnvironment(nullptr),
            musicFilenames(std::move(musicFilenames)),
            currentMusicIndex(0),
            isPaused(false) {

    }

    MusicPool::MusicPool(const std::string& resourcesMusicsDirectory) :
            soundEnvironment(nullptr),
            currentMusicIndex(0),
            isPaused(false) {
        std::string musicsFullPathDirectory = FileSystem::instance().getResourcesDirectory() + resourcesMusicsDirectory;
        for (const std::string& musicFullPath : FileUtil::getFilesRecursive(musicsFullPathDirectory)) {
            musicFilenames.push_back(FileUtil::getRelativePath(FileSystem::instance().getResourcesDirectory(), musicFullPath));
        }
        if (musicFilenames.empty()) {
            throw std::runtime_error("No musics found in directory: " + musicsFullPathDirectory);
        }
    }

    MusicPool::~MusicPool() {
        for (const auto& music : musics) {
            soundEnvironment->removeSoundComponent(*music.soundComponent);
        }
        musics.clear();
    }

    void MusicPool::setup(SoundEnvironment& soundEnvironment) {
        if (musicFilenames.empty()) {
            throw std::runtime_error("At least one music is required");
        }
        for (const std::string& musicFilename : musicFilenames) {
            std::shared_ptr<SoundComponent> soundComponent = SoundBuilder(soundEnvironment).newManualMusic(musicFilename, PlayBehavior::PLAY_ONCE);
            const AudioController& audioController = soundEnvironment.getAudioController(*soundComponent);
            musics.emplace_back(MusicInstance{soundComponent, audioController});
        }

        this->soundEnvironment = &soundEnvironment;
        this->currentMusicIndex = musics.size() - 1;
    }

    void MusicPool::pause() {
        musics[currentMusicIndex].soundComponent->getManualTrigger().pauseAll();
        isPaused = true;
    }

    void MusicPool::unpause() {
        musics[currentMusicIndex].soundComponent->getManualTrigger().unpauseAll();
        isPaused = false;
    }

    void MusicPool::refresh() {
        if (!isPaused) {
            if (musics[currentMusicIndex].audioController.getPlayersCount() == 0) {
                currentMusicIndex = (currentMusicIndex + 1) % (unsigned int)musics.size();
                musics[currentMusicIndex].soundComponent->getManualTrigger().playNew();
            }
        }
    }

}
