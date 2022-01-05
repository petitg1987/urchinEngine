#pragma once

#include <memory>
#include <SoundEnvironment.h>
#include <sound/global/GlobalSound.h>
#include <trigger/ShapeTrigger.h>

namespace urchin {

    class ZoneSound {
        public:
            ZoneSound(SoundEnvironment&, std::shared_ptr<GlobalSound>, std::shared_ptr<ShapeTrigger>);
            ~ZoneSound();

            GlobalSound& getSound() const;
            ShapeTrigger& getTrigger() const;

        private:
            SoundEnvironment& soundEnvironment;
            std::shared_ptr<GlobalSound> sound;
            std::shared_ptr<ShapeTrigger> trigger;
    };

}