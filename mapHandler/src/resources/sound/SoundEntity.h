#pragma once

#include <string>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundEntity {
        public:
            friend class Map;

            SoundEntity();
            ~SoundEntity();

            std::string getName() const;
            void setName(const std::string&);

            Sound* getSound() const;
            SoundTrigger* getSoundTrigger() const;
            void setSoundElements(const std::shared_ptr<Sound>&, const std::shared_ptr<SoundTrigger>&);
            void changeSoundTrigger(const std::shared_ptr<SoundTrigger>&);

        private:
            void setup(SoundEnvironment&);

            SoundEnvironment* soundEnvironment;

            std::string name;
            std::shared_ptr<Sound> sound;
            std::shared_ptr<SoundTrigger> soundTrigger;
    };

}
