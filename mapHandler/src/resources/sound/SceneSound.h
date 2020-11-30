#ifndef URCHINENGINE_SCENESOUND_H
#define URCHINENGINE_SCENESOUND_H

#include <string>
#include "UrchinSoundEngine.h"

namespace urchin {

    /**
    * Represent a sound and sound trigger on the scene
    */
    class SceneSound {
        //XML attributes
        #define NAME_ATTR "name"

        //XML tags
        #define SOUND_TAG "sound"
        #define SOUND_TRIGGER_TAG "soundTrigger"

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
            void loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            void writeOn(const std::shared_ptr<XmlChunk>&, XmlWriter&) const;

            SoundManager* soundManager;

            std::string name;
            Sound* sound;
            SoundTrigger* soundTrigger;

    };

}

#endif
