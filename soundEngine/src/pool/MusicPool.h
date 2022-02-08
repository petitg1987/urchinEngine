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

    class MusicPool { //TODO rename + rename directory ?
        public:
            explicit MusicPool(std::vector<std::string>);
            explicit MusicPool(const std::string&);
            ~MusicPool();

            void setup(SoundEnvironment&);

            void pause();
            void unpause();

            void refresh();

        private:
            SoundEnvironment* soundEnvironment;
            std::vector<std::string> musicFilenames;
            std::vector<MusicInstance> musics;
            std::size_t currentMusicIndex;
            bool isPaused;
    };

}
