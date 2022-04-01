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
            void setName(std::string);

            SoundComponent* getSoundComponent() const;
            void setSoundComponent(const std::shared_ptr<SoundComponent>&);

        private:
            void setup(SoundEnvironment*);

            SoundEnvironment* soundEnvironment;

            std::string name;
            std::shared_ptr<SoundComponent> soundComponent;
    };

}
