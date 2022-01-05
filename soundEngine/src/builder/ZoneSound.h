#pragma once

#include <memory>
#include <SoundEnvironment.h>
#include <sound/global/GlobalSound.h>
#include <trigger/ZoneTrigger.h>

namespace urchin {

    class ZoneSound {
        public:
            ZoneSound(SoundEnvironment&, std::shared_ptr<GlobalSound>, std::shared_ptr<ZoneTrigger>);
            ~ZoneSound();

            GlobalSound& getSound() const;
            ZoneTrigger& getTrigger() const;

        private:
            SoundEnvironment& soundEnvironment;
            std::shared_ptr<GlobalSound> sound;
            std::shared_ptr<ZoneTrigger> trigger;
    };

}