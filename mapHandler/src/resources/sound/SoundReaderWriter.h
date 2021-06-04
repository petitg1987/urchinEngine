#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundReaderWriter {
        //XML attributes
        #define TYPE_ATTR "type"

        //XML value
        #define POINT_VALUE "POINT"
        #define AMBIENT_VALUE "AMBIENT"

        //XML tags
        #define FILENAME_TAG "filename"
        #define VOLUME_TAG "volume"
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
