#ifndef URCHINENGINE_LIGHTREADERWRITER_H
#define URCHINENGINE_LIGHTREADERWRITER_H

#include <memory>

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

namespace urchin {

    class LightReaderWriter {
        //XML attributes
        #define TYPE_ATTR "type"

        //XML value
        #define OMNIDIRECTIONAL_VALUE "OMNIDIRECTIONAL"
        #define SUN_VALUE "SUN"

        //XML tags
        #define DIRECTION_TAG "direction"
        #define POSITION_TAG "position"
        #define AMBIENT_COLOR_TAG "ambientColor"
        #define EXPONENTIAL_ATTENUATION_TAG "exponentialAttenuation"
        #define PRODUCE_SHADOW_TAG "produceShadow"

        public:
            Light *loadFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeOn(const std::shared_ptr<XmlChunk> &, const Light *, XmlWriter &) const;

        private:
            Light *buildLightFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void buildChunkFrom(const std::shared_ptr<XmlChunk> &, const Light *, XmlWriter &) const;

            void loadPropertiesOn(Light *, const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writePropertiesOn(const std::shared_ptr<XmlChunk> &, const Light *, XmlWriter &) const;

            void loadFlagsOn(Light *, const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeFlagsOn(const std::shared_ptr<XmlChunk> &, const Light *, XmlWriter &) const;
    };

}

#endif
