#include <utility>
#include <random>

#include <MusicLoopPlayer.h>
#include <SoundBuilder.h>
#include <SoundEnvironment.h>

namespace urchin {

    MusicLoopPlayer::MusicLoopPlayer(std::vector<std::string> musicFilenames, MusicLoopStart musicLoopStart) :
            soundEnvironment(nullptr),
            musicFilenames(std::move(musicFilenames)),
            musicLoopStart(musicLoopStart),
            currentMusicIndex(0),
            isPaused(false) {
        if (musicFilenames.empty()) {
            throw std::runtime_error("At least one music is required");
        }
    }

    MusicLoopPlayer::MusicLoopPlayer(const std::string& resourcesMusicsDirectory, MusicLoopStart musicLoopStart) :
            soundEnvironment(nullptr),
            musicLoopStart(musicLoopStart),
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
    }

    MusicLoopPlayer::~MusicLoopPlayer() {
        for (const auto& music : musics) {
            soundEnvironment->removeSoundComponent(*music.soundComponent);
        }
        musics.clear();
    }

    void MusicLoopPlayer::setup(SoundEnvironment& soundEnvironment) {
        this->soundEnvironment = &soundEnvironment;

        for (const std::string& musicFilename : musicFilenames) {
            std::shared_ptr<SoundComponent> soundComponent = SoundBuilder(soundEnvironment).newManualMusic(musicFilename, PlayBehavior::PLAY_ONCE);
            const AudioController& audioController = soundEnvironment.getAudioController(*soundComponent);
            musics.emplace_back(MusicInstance{soundComponent, audioController});
        }

        if (musicLoopStart == MusicLoopStart::FIRST_MUSIC) {
            this->currentMusicIndex = musics.size() - 1;
        } else if (musicLoopStart == MusicLoopStart::RANDOM_MUSIC) {
            std::mt19937 mt(std::random_device{}());
            std::uniform_int_distribution<std::size_t> distribution(0, musics.size() - 1);
            this->currentMusicIndex = distribution(mt);
        } else {
            throw std::runtime_error("Unknown loop start type: " + std::to_string(musicLoopStart));
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
        if (!isPaused) {
            if (musics[currentMusicIndex].audioController.getPlayersCount() == 0) {
                currentMusicIndex = (currentMusicIndex + 1) % (unsigned int)musics.size();
                musics[currentMusicIndex].soundComponent->getManualTrigger().playNew();
            }
        }
    }

}
