#pragma once

#include <sound/Sound.h>
#include <trigger/SoundTrigger.h>
#include <trigger/ManualTrigger.h>
#include <sound/spatial/SpatialSound.h>

namespace urchin {

    class SoundComponent {
        public:
            SoundComponent(std::shared_ptr<Sound>, std::shared_ptr<SoundTrigger>);

            Sound& getSound() const;
            SpatialSound& getSpatialSound() const;

            SoundTrigger& getSoundTrigger() const;
            ManualTrigger& getManualTrigger() const;

        private:
            std::shared_ptr<Sound> sound;
            std::shared_ptr<SoundTrigger> soundTrigger;
    };

}