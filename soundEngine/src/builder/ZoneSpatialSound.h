#pragma once

#include <memory>
#include <SoundEnvironment.h>
#include <sound/spatial/SpatialSound.h>
#include <trigger/ShapeTrigger.h>

namespace urchin {

    class ZoneSpatialSound {
        public:
            ZoneSpatialSound(SoundEnvironment&, std::shared_ptr<SpatialSound>, std::shared_ptr<ShapeTrigger>);
            ~ZoneSpatialSound();

            SpatialSound& getSound() const;
            ShapeTrigger& getTrigger() const;

        private:
            SoundEnvironment& soundEnvironment;
            std::shared_ptr<SpatialSound> sound;
            std::shared_ptr<ShapeTrigger> trigger;
    };

}