#pragma once

#include <SoundEnvironment.h>
#include <builder/ManualSound.h>
#include <builder/ManualSpatialSound.h>
#include <builder/ZoneSound.h>
#include <builder/ZoneSpatialSound.h>

namespace urchin {

    class SoundBuilder {
        public:
            explicit SoundBuilder(SoundEnvironment&);

            //usage examples: UI sounds, ambient musics
            std::unique_ptr<ManualSound> newManualEffect(std::string, PlayBehavior);
            std::unique_ptr<ManualSound> newManualMusic(std::string, PlayBehavior);

            //usage examples: sound of an opening door, player turn on a music radio
            std::unique_ptr<ManualSpatialSound> newManualSpatialEffect(std::string, const Point3<float>&, float, PlayBehavior);
            std::unique_ptr<ManualSpatialSound> newManualSpatialMusic(std::string, const Point3<float>&, float, PlayBehavior);

            //usage example: user reach end of level and the success sound is played
            std::unique_ptr<ZoneSound> newZoneEffect(std::string, std::shared_ptr<ShapeTrigger>);
            std::unique_ptr<ZoneSound> newZoneMusic(std::string, std::shared_ptr<ShapeTrigger>);

            //usage examples: machine making noise, radio playing musics
            std::unique_ptr<ZoneSpatialSound> newZoneSpatialEffect(std::string, const Point3<float>&, float, std::shared_ptr<ShapeTrigger>);
            std::unique_ptr<ZoneSpatialSound> newZoneSpatialMusic(std::string, const Point3<float>&, float, std::shared_ptr<ShapeTrigger>);

        private:
            SoundEnvironment& soundEnvironment;
    };

}