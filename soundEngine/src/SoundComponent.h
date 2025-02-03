#pragma once

#include <sound/Sound.h>
#include <trigger/SoundTrigger.h>
#include <trigger/ManualTrigger.h>
#include <trigger/AreaTrigger.h>
#include <sound/global/GlobalSound.h>
#include <sound/localizable/LocalizableSound.h>

namespace urchin {

    class SoundComponent {
        public:
            SoundComponent(std::shared_ptr<Sound>, std::shared_ptr<SoundTrigger>);
            SoundComponent(const SoundComponent&);
            SoundComponent& operator=(const SoundComponent&) = delete;

            Sound& getSound() const;
            GlobalSound& getGlobalSound() const;
            LocalizableSound& getLocalizableSound() const;

            SoundTrigger& getSoundTrigger() const;
            ManualTrigger& getManualTrigger() const;
            AreaTrigger& getAreaTrigger() const;

        private:
            std::shared_ptr<Sound> sound;
            std::shared_ptr<SoundTrigger> soundTrigger;
    };

}