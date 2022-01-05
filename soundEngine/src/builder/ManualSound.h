#pragma once

#include <memory>
#include <SoundEnvironment.h>
#include <sound/global/GlobalSound.h>
#include <trigger/ManualTrigger.h>

namespace urchin {

    class ManualSound {
        public:
            ManualSound(SoundEnvironment&, std::shared_ptr<GlobalSound>, std::shared_ptr<ManualTrigger>);
            ~ManualSound();

            GlobalSound& getSound() const;
            ManualTrigger& getTrigger() const;

        private:
            SoundEnvironment& soundEnvironment;
            std::shared_ptr<GlobalSound> sound;
            std::shared_ptr<ManualTrigger> trigger;
    };

}