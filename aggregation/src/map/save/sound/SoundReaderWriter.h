#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundReaderWriter {
        public:
            static std::unique_ptr<Sound> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const Sound&, UdaParser&);

        private:
            static constexpr char TYPE_ATTR[] = "type";
            static constexpr char CATEGORY_ATTR[] = "category";
            static constexpr char SPATIAL_VALUE[] = "SPATIAL";
            static constexpr char GLOBAL_VALUE[] = "GLOBAL";
            static constexpr char MUSIC_VALUE[] = "MUSIC";
            static constexpr char EFFECTS_VALUE[] = "EFFECTS";
            static constexpr char FILENAME_TAG[] = "filename";
            static constexpr char INITIAL_VOLUME_TAG[] = "initialVolume";
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char INAUDIBLE_DISTANCE_TAG[] = "inaudibleDistance";
    };

}
