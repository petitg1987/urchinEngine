#include <utility>
#include <random>

#include <MusicLoopPlayer.h>
#include <SoundBuilder.h>
#include <SoundEnvironment.h>

namespace urchin {

    MusicLoopPlayer::MusicLoopPlayer(std::vector<std::string> musicFilenames, unsigned int musicStartIndex) :
            soundEnvironment(nullptr),
            musicFilenames(std::move(musicFilenames)),
            currentMusicIndex(0),
            isPaused(false) {
        if (musicFilenames.empty()) {
            throw std::runtime_error("At least one music is required");
        }
        initialize(musicStartIndex);
    }

    MusicLoopPlayer::MusicLoopPlayer(const std::string& resourcesMusicsDirectory, unsigned int musicStartIndex) :
            soundEnvironment(nullptr),
            currentMusicIndex(0),
            isPaused(false) {
        std::string musicsFullPathDirectory = FileSystem::instance().getResourcesDirectory() + resourcesMusicsDirectory;
        for (const std::string& musicFullPath : FileUtil::getFiles(musicsFullPathDirectory)) {
            musicFilenames.push_back(FileUtil::getRelativePath(FileSystem::instance().getResourcesDirectory(), musicFullPath));
        }
        if (musicFilenames.empty()) {
            throw std::runtime_error("No musics found in directory: " + musicsFullPathDirectory);
        }
        std::ranges::sort(musicFilenames);
        initialize(musicStartIndex);
    }

    MusicLoopPlayer::~MusicLoopPlayer() {
        for (const auto& music : musics) {
            soundEnvironment->removeSoundComponent(*music.soundComponent);
        }
        musics.clear();
    }

    void MusicLoopPlayer::initialize(unsigned int musicStartIndex) {
        if (musicStartIndex == 0) {
            currentMusicIndex = musicFilenames.size() - 1;
        } else {
            currentMusicIndex = (musicStartIndex - 1) % (unsigned int)musicFilenames.size();
        }
    }

    void MusicLoopPlayer::setup(SoundEnvironment& soundEnvironment) {
        this->soundEnvironment = &soundEnvironment;

        for (const std::string& musicFilename : musicFilenames) {
            std::shared_ptr<SoundComponent> soundComponent = SoundBuilder(soundEnvironment).newManualMusic(musicFilename, PlayBehavior::PLAY_ONCE);
            const AudioController& audioController = soundEnvironment.getAudioController(*soundComponent);
            musics.emplace_back(MusicInstance{soundComponent, audioController});
        }
    }

    void MusicLoopPlayer::pause() {
        musics[currentMusicIndex].soundComponent->getManualTrigger().pauseAll();
        isPaused = true;
    }

    void MusicLoopPlayer::unpause() {
        musics[currentMusicIndex].soundComponent->getManualTrigger().unpauseAll();
        isPaused = false;
    }

    void MusicLoopPlayer::refresh() {
        if (!isPaused && musics[currentMusicIndex].audioController.getPlayersCount() == 0) {
            currentMusicIndex = (currentMusicIndex + 1) % (unsigned int)musics.size();
            musics[currentMusicIndex].soundComponent->getManualTrigger().playNew();
        }
    }

}
