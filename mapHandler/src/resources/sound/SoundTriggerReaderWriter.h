#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundTriggerReaderWriter {
        //XML attributes
        #define TYPE_ATTR "type"

        //XML value
        #define MANUAL_VALUE "MANUAL"
        #define SHAPE_VALUE "SHAPE"
        #define PLAY_ONCE_VALUE "PLAY_ONCE"
        #define PLAY_LOOP_VALUE "PLAY_LOOP"
        #define INSTANT_STOP_VALUE "INSTANT_STOP"
        #define SMOOTH_STOP_VALUE "SMOOTH_STOP"

        //XML tags
        #define SOUND_BEHAVIOR_TAG "soundBehavior"
        #define PLAY_BEHAVIOR_TAG "playBehavior"
        #define SOUND_SHAPE_TAG "soundShape"

        public:
            static SoundTrigger* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOn(const std::shared_ptr<XmlChunk>&, const SoundTrigger*, XmlWriter&);

        private:
            static SoundTrigger* buildSoundTriggerFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void buildChunkFrom(const std::shared_ptr<XmlChunk>&, const SoundTrigger*, XmlWriter&);

            static SoundBehavior buildSoundBehaviorFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void buildChunkFrom(const std::shared_ptr<XmlChunk>&, const SoundBehavior&, XmlWriter&);
    };

}
