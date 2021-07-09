#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundTriggerReaderWriter {
        public:
            static SoundTrigger* loadFrom(const XmlChunk*, const DataParser&);
            static void writeOn(XmlChunk*, const SoundTrigger*, XmlWriter&);

        private:
            static SoundTrigger* buildSoundTriggerFrom(const XmlChunk*, const DataParser&);
            static void buildChunkFrom(XmlChunk*, const SoundTrigger*, XmlWriter&);

            static SoundTrigger::PlayBehavior loadPlayBehaviorFrom(const XmlChunk*, const DataParser&);
            static void writePlayBehaviorFrom(const XmlChunk*, SoundTrigger::PlayBehavior, XmlWriter&);

            static constexpr char TYPE_ATTR[] = "type";
            static constexpr char MANUAL_VALUE[] = "MANUAL";
            static constexpr char SHAPE_VALUE[] = "SHAPE";
            static constexpr char PLAY_ONCE_VALUE[] = "PLAY_ONCE";
            static constexpr char PLAY_LOOP_VALUE[] = "PLAY_LOOP";
            static constexpr char PLAY_BEHAVIOR_TAG[] = "playBehavior";
            static constexpr char SOUND_SHAPE_TAG[] = "soundShape";
    };

}
