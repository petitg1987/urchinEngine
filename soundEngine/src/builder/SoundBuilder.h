#pragma once

#include <SoundEnvironment.h>
#include <builder/SoundWrapper.h>
#include <sound/global/GlobalSound.h>
#include <sound/spatial/SpatialSound.h>
#include <trigger/ManualTrigger.h>
#include <trigger/ShapeTrigger.h>

namespace urchin {

    class SoundBuilder {
        public:
            explicit SoundBuilder(SoundEnvironment&);

            //usage examples: UI sounds, ambient musics
            std::unique_ptr<SoundWrapper<GlobalSound, ManualTrigger>> newManualEffect(std::string, PlayBehavior);
            std::unique_ptr<SoundWrapper<GlobalSound, ManualTrigger>> newManualMusic(std::string, PlayBehavior);

            //usage examples: sound of opening door, player turn on a radio
            std::unique_ptr<SoundWrapper<SpatialSound, ManualTrigger>> newManualSpatialEffect(std::string, const Point3<float>&, float, PlayBehavior);
            std::unique_ptr<SoundWrapper<SpatialSound, ManualTrigger>> newManualSpatialMusic(std::string, const Point3<float>&, float, PlayBehavior);

            //usage examples: a machine making noise, radio playing music
            std::unique_ptr<SoundWrapper<SpatialSound, ShapeTrigger>> newSpatialEffect(std::string, const Point3<float>&, float, std::shared_ptr<ShapeTrigger>);
            std::unique_ptr<SoundWrapper<SpatialSound, ShapeTrigger>> newSpatialMusic(std::string, const Point3<float>&, float, std::shared_ptr<ShapeTrigger>);

        private:
            SoundEnvironment& soundEnvironment;
    };

}