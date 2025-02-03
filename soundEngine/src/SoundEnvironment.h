#pragma once

#include <vector>
#include <thread>
#include <memory>
#include <UrchinCommon.h>

#include <AudioController.h>
#include <sound/Sound.h>
#include <player/stream/StreamUpdateWorker.h>
#include <SoundComponent.h>
#include <MusicLoopPlayer.h>
#include <SoundBuilder.h>

namespace urchin {

    class SoundEnvironment {
        public:
            SoundEnvironment();
            ~SoundEnvironment();

            SoundBuilder& getBuilder();
            void addSoundComponent(std::shared_ptr<SoundComponent>);
            void removeSoundComponent(const SoundComponent*);
            const AudioController& getAudioController(const SoundComponent&) const;

            void addMusicLoopPlayer(std::shared_ptr<MusicLoopPlayer>);
            void removeMusicLoopPlayer(const MusicLoopPlayer&);

            void setupSoundsVolume(Sound::SoundCategory, float);
            void setMasterVolume(float) const;
            float getMasterVolume() const;

            void pause() const;
            void unpause() const;

            void process(const Point3<float>&, const Vector3<float>&, const Vector3<float>&) const;
            void process() const;

        private:
            SoundBuilder soundBuilder;

            std::vector<std::unique_ptr<AudioController>> audioControllers;
            std::vector<std::shared_ptr<MusicLoopPlayer>> musicLoopPlayers;
            std::map<Sound::SoundCategory, float> soundVolumes;

            //stream chunk updater thread
            StreamUpdateWorker streamUpdateWorker;
            std::jthread streamUpdateWorkerThread;
    };

}
