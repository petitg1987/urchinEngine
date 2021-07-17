#pragma once

#include <vector>
#include <thread>
#include <memory>
#include <UrchinCommon.h>

#include <AudioController.h>
#include <device/DeviceManager.h>
#include <sound/Sound.h>
#include <trigger/SoundTrigger.h>
#include <player/stream/StreamUpdateWorker.h>

namespace urchin {

    class SoundEnvironment {
        public:
            SoundEnvironment();
            ~SoundEnvironment();

            void addSound(std::shared_ptr<Sound>, std::shared_ptr<SoundTrigger>);
            void removeSound(const Sound&);
            void changeSoundTrigger(const Sound&, std::shared_ptr<SoundTrigger>);
            void setupSoundsVolume(Sound::SoundCategory, float);

            void setMasterVolume(float);
            float getMasterVolume() const;

            void pause();
            void unpause();

            void checkNoExceptionRaised();
            void process(const Point3<float>&);
            void process();

        private:
            void adjustSoundVolume(Sound&);

            std::vector<std::unique_ptr<AudioController>> audioControllers;
            std::map<Sound::SoundCategory, float> soundVolumes;

            //stream chunk updater thread
            std::unique_ptr<StreamUpdateWorker> streamUpdateWorker;
            std::unique_ptr<std::thread> streamUpdateWorkerThread;
    };

}
