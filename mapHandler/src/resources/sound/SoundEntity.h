#pragma once

#include <string>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundEntity {
        public:
            friend class Map;

            SoundEntity();
            ~SoundEntity();

            void setup(SoundEnvironment*);

            std::string getName() const;
            void setName(const std::string&);

            Sound* getSound() const;
            SoundTrigger* getSoundTrigger() const;
            void setSoundElements(const std::shared_ptr<Sound>&, const std::shared_ptr<SoundTrigger>&);
            void changeSoundTrigger(const std::shared_ptr<SoundTrigger>&);

        private:
            void loadFrom(const UdaChunk*, const UdaParser&);
            void writeOn(UdaChunk&, UdaWriter&) const;

            static constexpr char NAME_ATTR[] = "name";
            static constexpr char SOUND_TAG[] = "sound";
            static constexpr char SOUND_TRIGGER_TAG[] = "soundTrigger";

            SoundEnvironment* soundEnvironment;

            std::string name;
            std::shared_ptr<Sound> sound;
            std::shared_ptr<SoundTrigger> soundTrigger;
    };

}
