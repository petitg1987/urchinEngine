#pragma once

#include <SoundEnvironment.h>
#include <SoundComponent.h>
#include <trigger/ZoneTrigger.h>

namespace urchin {

    class SoundBuilder {
        public:
            explicit SoundBuilder(SoundEnvironment&);

            //usage examples: UI sounds, ambient musics
            std::shared_ptr<SoundComponent> newManualEffect(std::string, PlayBehavior);
            std::shared_ptr<SoundComponent> newManualMusic(std::string, PlayBehavior);

            //usage examples: sound of an opening door, player turn on a music radio
            std::shared_ptr<SoundComponent> newManualSpatialEffect(std::string, const Point3<float>&, float, PlayBehavior);
            std::shared_ptr<SoundComponent> newManualSpatialMusic(std::string, const Point3<float>&, float, PlayBehavior);

            //usage example: user reach end of level and the success sound is played
            std::shared_ptr<SoundComponent> newZoneEffect(std::string, std::shared_ptr<ZoneTrigger>);
            std::shared_ptr<SoundComponent> newZoneMusic(std::string, std::shared_ptr<ZoneTrigger>);

            //usage examples: machine making noise, radio playing musics
            std::shared_ptr<SoundComponent> newZoneSpatialEffect(std::string, const Point3<float>&, float, std::shared_ptr<ZoneTrigger>);
            std::shared_ptr<SoundComponent> newZoneSpatialMusic(std::string, const Point3<float>&, float, std::shared_ptr<ZoneTrigger>);

        private:
            std::shared_ptr<SoundComponent> buildSoundComponent(std::shared_ptr<Sound>, std::shared_ptr<SoundTrigger>);

            SoundEnvironment& soundEnvironment;
    };

}