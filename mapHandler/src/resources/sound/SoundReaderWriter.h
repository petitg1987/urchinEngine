#ifndef URCHINENGINE_SOUNDREADERWRITER_H
#define URCHINENGINE_SOUNDREADERWRITER_H

#include <memory>
#include <string>

#include "UrchinCommon.h"
#include "UrchinSoundEngine.h"

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
            Sound *loadFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeOn(const std::shared_ptr<XmlChunk> &, const Sound *, XmlWriter &) const;

        private:
            Sound *buildSoundFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void buildChunkFrom(const std::shared_ptr<XmlChunk> &, const Sound *, XmlWriter &) const;

            void loadPropertiesOn(Sound *, const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writePropertiesOn(const std::shared_ptr<XmlChunk> &, const Sound *, XmlWriter &) const;
    };

}

#endif
