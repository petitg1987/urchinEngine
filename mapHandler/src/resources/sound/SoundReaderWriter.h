#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundReaderWriter {
        //XML attributes
        #define TYPE_ATTR "type"
        #define CATEGORY_ATTR "category"

        //XML value
        #define SPATIAL_VALUE "SPATIAL"
        #define GLOBAL_VALUE "GLOBAL"
        #define MUSIC_VALUE "MUSIC"
        #define EFFECTS_VALUE "EFFECTS"

        //XML tags
        #define FILENAME_TAG "filename"
        #define INITIAL_VOLUME_TAG "initialVolume"
        #define POSITION_TAG "position"
        #define INAUDIBLE_DISTANCE_TAG "inaudibleDistance"

        public:
            static Sound* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOn(const std::shared_ptr<XmlChunk>&, const Sound*, XmlWriter&);

        private:
            static Sound* buildSoundFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void buildChunkFrom(const std::shared_ptr<XmlChunk>&, const Sound*, XmlWriter&);

            static void loadPropertiesOn(Sound*, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writePropertiesOn(const std::shared_ptr<XmlChunk>&, const Sound*, XmlWriter&);
    };

}
