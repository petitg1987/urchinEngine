#pragma once

#include <string>
#include <vector>
#include <memory>

#include <SoundComponent.h>
#include <AudioController.h>

namespace urchin {

     class SoundEnvironment;

    struct MusicInstance {
        std::shared_ptr<SoundComponent> soundComponent;
        const AudioController& audioController;
    };

    enum MusicLoopStart {
        FIRST_MUSIC,
        RANDOM_MUSIC
    };

    class MusicLoopPlayer {
        public:
            MusicLoopPlayer(std::vector<std::string>, MusicLoopStart);
            MusicLoopPlayer(const std::string&, MusicLoopStart);
            ~MusicLoopPlayer();

            void setup(SoundEnvironment&);

            void pause();
            void unpause();

            void refresh();

        private:
            SoundEnvironment* soundEnvironment;
            std::vector<std::string> musicFilenames;
            MusicLoopStart musicLoopStart;

            std::vector<MusicInstance> musics;
            std::size_t currentMusicIndex;
            bool isPaused;
    };

}
