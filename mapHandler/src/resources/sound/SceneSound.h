#pragma once

#include <string>
#include <UrchinSoundEngine.h>

namespace urchin {

    /**
    * Represent a sound and sound trigger on the scene
    */
    class SceneSound {
        public:
            friend class Map;

            SceneSound();
            ~SceneSound();

            void setSoundManager(SoundManager*);

            std::string getName() const;
            void setName(const std::string&);

            Sound* getSound() const;
            SoundTrigger* getSoundTrigger() const;
            void setSoundElements(Sound*, SoundTrigger*);
            void changeSoundTrigger(SoundTrigger*);

        private:
            void loadFrom(const XmlChunk*, const DataParser&);
            void writeOn(XmlChunk*, XmlWriter&) const;

            static constexpr char NAME_ATTR[] = "name";
            static constexpr char SOUND_TAG[] = "sound";
            static constexpr char SOUND_TRIGGER_TAG[] = "soundTrigger";

            SoundManager* soundManager;

            std::string name;
            Sound* sound;
            SoundTrigger* soundTrigger;

    };

}
