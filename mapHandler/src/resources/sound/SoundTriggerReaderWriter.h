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

        //XML tags
        #define PLAY_BEHAVIOR_TAG "playBehavior"
        #define SOUND_SHAPE_TAG "soundShape"

        public:
            static SoundTrigger* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOn(const std::shared_ptr<XmlChunk>&, const SoundTrigger*, XmlWriter&);

        private:
            static SoundTrigger* buildSoundTriggerFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void buildChunkFrom(const std::shared_ptr<XmlChunk>&, const SoundTrigger*, XmlWriter&);

            static SoundTrigger::PlayBehavior loadPlayBehaviorFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writePlayBehaviorFrom(const std::shared_ptr<XmlChunk>&, SoundTrigger::PlayBehavior, XmlWriter&);
    };

}
