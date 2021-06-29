#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundReaderWriter {
        public:
            static Sound* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOn(const std::shared_ptr<XmlChunk>&, const Sound*, XmlWriter&);

        private:
            static Sound* buildSoundFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void buildChunkFrom(const std::shared_ptr<XmlChunk>&, const Sound*, XmlWriter&);

            static void loadPropertiesOn(Sound*, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writePropertiesOn(const std::shared_ptr<XmlChunk>&, const Sound*, XmlWriter&);

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
