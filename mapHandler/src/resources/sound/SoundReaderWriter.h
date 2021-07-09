#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundReaderWriter {
        public:
            static Sound* loadFrom(const XmlChunk*, const DataParser&);
            static void writeOn(XmlChunk*, const Sound*, DataWriter&);

        private:
            static Sound* buildSoundFrom(const XmlChunk*, const DataParser&);
            static void buildChunkFrom(XmlChunk*, const Sound*, DataWriter&);

            static void loadPropertiesOn(Sound*, const XmlChunk*, const DataParser&);
            static void writePropertiesOn(const XmlChunk*, const Sound*, DataWriter&);

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
