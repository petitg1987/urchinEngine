#pragma once

#include <string>
#include <vector>
#include <memory>

#include <SoundComponent.h>
#include <SoundEnvironment.h>

namespace urchin {

    struct MusicInstance {
        std::shared_ptr<SoundComponent> soundComponent;
        const AudioController& audioController;
    };

    class MusicPool { //TODO rename + rename directory ?
        public:
            explicit MusicPool(SoundEnvironment&, const std::vector<std::string>&);
            explicit MusicPool(SoundEnvironment&, const std::string&);
            ~MusicPool();

            void pause();
            void unpause();

            void process();

        private:
            void initialize(const std::vector<std::string>&);

            SoundEnvironment& soundEnvironment;

            std::vector<MusicInstance> musics;
            std::size_t currentMusicIndex;
            bool isPaused;
    };

}