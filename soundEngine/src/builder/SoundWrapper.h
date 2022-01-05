#pragma once

#include <memory>
#include <SoundEnvironment.h>

namespace urchin {

    template<class SOUND, class TRIGGER> class SoundWrapper {
        public:
            SoundWrapper(SoundEnvironment&, std::shared_ptr<SOUND>, std::shared_ptr<TRIGGER>);
            ~SoundWrapper();

            SOUND& getSound() const;
            TRIGGER& getSoundTrigger() const;

        private:
            SoundEnvironment& soundEnvironment;
            std::shared_ptr<SOUND> sound;
            std::shared_ptr<TRIGGER> soundTrigger;
    };

    #include "SoundWrapper.inl"

}