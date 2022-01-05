#pragma once

#include <memory>
#include <SoundEnvironment.h>
#include <sound/spatial/SpatialSound.h>
#include <trigger/ZoneTrigger.h>

namespace urchin {

    class ZoneSpatialSound {
        public:
            ZoneSpatialSound(SoundEnvironment&, std::shared_ptr<SpatialSound>, std::shared_ptr<ZoneTrigger>);
            ~ZoneSpatialSound();

            SpatialSound& getSound() const;
            ZoneTrigger& getTrigger() const;

        private:
            SoundEnvironment& soundEnvironment;
            std::shared_ptr<SpatialSound> sound;
            std::shared_ptr<ZoneTrigger> trigger;
    };

}