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

    class MusicLoopPlayer {
        public:
            explicit MusicLoopPlayer(std::vector<std::string>, unsigned int musicStartIndex = 0);
            explicit MusicLoopPlayer(const std::string&, unsigned int musicStartIndex = 0);
            ~MusicLoopPlayer();

            void setup(SoundEnvironment&);

            void pause();
            void unpause();

            void refresh();

        private:
            void initialize(unsigned int);

            SoundEnvironment* soundEnvironment;
            std::vector<std::string> musicFilenames;

            std::vector<MusicInstance> musics;
            std::size_t currentMusicIndex;
            bool isPaused;
    };

}
