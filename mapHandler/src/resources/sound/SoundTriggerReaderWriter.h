#ifndef URCHINENGINE_SOUNDTRIGGERREADERWRITER_H
#define URCHINENGINE_SOUNDTRIGGERREADERWRITER_H

#include <memory>
#include <string>

#include "UrchinCommon.h"
#include "UrchinSoundEngine.h"

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
        #define STOP_BEHAVIOR_TAG "stopBehavior"
        #define VOLUME_DECREASE_PERCENTAGE_ON_STOP_TAG "volumeDecreasePercentageOnStop"
        #define SOUND_SHAPE_TAG "soundShape"

        public:
            SoundTrigger* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const;
            void writeOn(const std::shared_ptr<XmlChunk>&, const SoundTrigger*, XmlWriter&) const;

        private:
            SoundTrigger* buildSoundTriggerFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const;
            void buildChunkFrom(const std::shared_ptr<XmlChunk>&, const SoundTrigger*, XmlWriter&) const;

            SoundBehavior buildSoundBehaviorFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const;
            void buildChunkFrom(const std::shared_ptr<XmlChunk>&, const SoundBehavior&, XmlWriter&) const;
    };

}

#endif
