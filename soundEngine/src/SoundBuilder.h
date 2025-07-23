#pragma once

#include "SoundComponent.h"
#include "trigger/AreaTrigger.h"
#include "player/filereader/ChunkPreLoader.h"

namespace urchin {

    class SoundEnvironment;

    class SoundBuilder {
        public:
            explicit SoundBuilder(SoundEnvironment&);

            //usage examples: UI sounds, ambient musics
            std::shared_ptr<SoundComponent> newManualTriggerEffect(std::string, PlayBehavior);
            std::shared_ptr<SoundComponent> newManualTriggerMusic(std::string, PlayBehavior);

            //usage examples: sound of an opening door, player turn on a music radio
            std::shared_ptr<SoundComponent> newManualTriggerLocalizableEffect(std::string, const Point3<float>&, float, PlayBehavior);
            std::shared_ptr<SoundComponent> newManualTriggerLocalizableMusic(std::string, const Point3<float>&, float, PlayBehavior);

            //usage example: user reach end of level and the success sound is played
            std::shared_ptr<SoundComponent> newAreaTriggerEffect(std::string, std::shared_ptr<AreaTrigger>);
            std::shared_ptr<SoundComponent> newAreaTriggerMusic(std::string, std::shared_ptr<AreaTrigger>);

            //usage examples: machine making noise, radio playing musics
            std::shared_ptr<SoundComponent> newLocalizableEffect(std::string, const Point3<float>&, float, PlayBehavior);
            std::shared_ptr<SoundComponent> newLocalizableMusic(std::string, const Point3<float>&, float, PlayBehavior);

        private:
            std::shared_ptr<SoundComponent> buildSoundComponent(std::shared_ptr<Sound>, std::shared_ptr<SoundTrigger>) const;

            SoundEnvironment& soundEnvironment;
            ChunkPreLoader chunkPreLoader;
    };

}