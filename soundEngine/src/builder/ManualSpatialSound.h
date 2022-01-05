#pragma once

#include <memory>
#include <SoundEnvironment.h>
#include <sound/spatial/SpatialSound.h>
#include <trigger/ManualTrigger.h>

namespace urchin {

    class ManualSpatialSound {
        public:
            ManualSpatialSound(SoundEnvironment&, std::shared_ptr<SpatialSound>, std::shared_ptr<ManualTrigger>);
            ~ManualSpatialSound();

            SpatialSound& getSound() const;
            ManualTrigger& getTrigger() const;

        private:
            SoundEnvironment& soundEnvironment;
            std::shared_ptr<SpatialSound> sound;
            std::shared_ptr<ManualTrigger> trigger;
    };

}