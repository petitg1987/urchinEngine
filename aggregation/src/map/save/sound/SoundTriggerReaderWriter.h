#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundTriggerReaderWriter {
        public:
            static std::unique_ptr<SoundTrigger> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const SoundTrigger&, UdaParser&);

        private:
            static PlayBehavior loadPlayBehavior(const UdaChunk*, const UdaParser&);
            static void writePlayBehavior(UdaChunk&, PlayBehavior, UdaParser&);

            static constexpr char TYPE_ATTR[] = "type";
            static constexpr char MANUAL_VALUE[] = "MANUAL";
            static constexpr char SHAPE_VALUE[] = "SHAPE";
            static constexpr char PLAY_ONCE_VALUE[] = "PLAY_ONCE";
            static constexpr char PLAY_LOOP_VALUE[] = "PLAY_LOOP";
            static constexpr char PLAY_BEHAVIOR_TAG[] = "playBehavior";
            static constexpr char SOUND_SHAPE_TAG[] = "soundShape";
    };

}
