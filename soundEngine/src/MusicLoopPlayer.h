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

    enum class MusicPlayOrder {
        ALPHABETIC_ORDER,
        RANDOM_ORDER
    };

    class MusicLoopPlayer {
        public:
            MusicLoopPlayer(std::vector<std::string>, MusicPlayOrder);
            MusicLoopPlayer(const std::string&, MusicPlayOrder);
            ~MusicLoopPlayer();

            void setup(SoundEnvironment&);

            void pause();
            void unpause();

            void refresh();

        private:
            void orderMusics(MusicPlayOrder);

            SoundEnvironment* soundEnvironment;
            std::vector<std::string> musicFilenames;

            std::vector<MusicInstance> musics;
            std::size_t currentMusicIndex;
            bool isPaused;
    };

}
