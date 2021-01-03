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
            static Light* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOn(const std::shared_ptr<XmlChunk>&, const Light*, XmlWriter&);

        private:
            static Light* buildLightFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void buildChunkFrom(const std::shared_ptr<XmlChunk>&, const Light*, XmlWriter&);

            static void loadPropertiesOn(Light*, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writePropertiesOn(const std::shared_ptr<XmlChunk>&, const Light*, XmlWriter&);

            static void loadFlagsOn(Light*, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeFlagsOn(const std::shared_ptr<XmlChunk>&, const Light*, XmlWriter&);
    };

}

#endif
