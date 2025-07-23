#include <utility>
#include <algorithm>
#include <random>

#include "MusicLoopPlayer.h"
#include "SoundBuilder.h"
#include "SoundEnvironment.h"

namespace urchin {

    MusicLoopPlayer::MusicLoopPlayer(std::vector<std::string> musicFilenames, MusicPlayOrder playOrder) :
            soundEnvironment(nullptr),
            musicFilenames(std::move(musicFilenames)),
            currentMusicIndex(0),
            isPaused(false) {
        if (this->musicFilenames.empty()) {
            throw std::runtime_error("At least one music is required");
        }
        orderMusics(playOrder);
    }

    MusicLoopPlayer::MusicLoopPlayer(const std::string& resourcesMusicsDirectory, MusicPlayOrder playOrder) :
            soundEnvironment(nullptr),
            currentMusicIndex(0),
            isPaused(false) {
        std::string musicsFullPathDirectory = FileSystem::instance().getResourcesDirectory() + resourcesMusicsDirectory;
        for (const std::string& musicFilename : FileUtil::getFiles(musicsFullPathDirectory)) {
            musicFilenames.push_back(musicFilename);
        }
        if (musicFilenames.empty()) {
            throw std::runtime_error("No musics found in directory: " + musicsFullPathDirectory);
        }
        orderMusics(playOrder);
    }

    MusicLoopPlayer::~MusicLoopPlayer() {
        for (const auto& music : musics) {
            soundEnvironment->removeSoundComponent(music.soundComponent.get());
        }
        musics.clear();
    }

    void MusicLoopPlayer::orderMusics(MusicPlayOrder playOrder) {
        if (playOrder == MusicPlayOrder::ALPHABETIC_ORDER) {
            std::ranges::sort(musicFilenames);
        } else if (playOrder == MusicPlayOrder::RANDOM_ORDER) {
            std::mt19937 rng(std::random_device{}());
            std::ranges::shuffle(musicFilenames, rng);
        } else {
            throw std::runtime_error("Unknown music play order: " + std::to_string((int)playOrder));
        }

        currentMusicIndex = musicFilenames.size() - 1;
    }

    void MusicLoopPlayer::setup(SoundEnvironment& soundEnvironment) {
        this->soundEnvironment = &soundEnvironment;

        for (const std::string& musicFilename : musicFilenames) {
            std::shared_ptr<SoundComponent> soundComponent = soundEnvironment.getBuilder().newManualTriggerMusic(musicFilename, PlayBehavior::PLAY_ONCE);
            const AudioController& audioController = soundEnvironment.getAudioController(*soundComponent);
            musics.emplace_back(MusicInstance{.soundComponent=soundComponent, .audioController=audioController});
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
