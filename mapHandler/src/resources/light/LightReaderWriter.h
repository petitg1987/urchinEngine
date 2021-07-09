#pragma once

#include <memory>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

namespace urchin {

    class LightReaderWriter {
        public:
            static Light* loadFrom(const XmlChunk*, const XmlParser&);
            static void writeOn(XmlChunk*, const Light*, XmlWriter&);

        private:
            static Light* buildLightFrom(const XmlChunk*, const XmlParser&);
            static void buildChunkFrom(XmlChunk*, const Light*, XmlWriter&);

            static void loadPropertiesFrom(Light*, const XmlChunk*, const XmlParser&);
            static void writePropertiesOn(const XmlChunk*, const Light*, XmlWriter&);

            static void loadFlagsFrom(Light*, const XmlChunk*, const XmlParser&);
            static void writeFlagsOn(const XmlChunk*, const Light*, XmlWriter&);

            static constexpr char TYPE_ATTR[] = "type";
            static constexpr char OMNIDIRECTIONAL_VALUE[] = "OMNIDIRECTIONAL";
            static constexpr char SUN_VALUE[] = "SUN";
            static constexpr char DIRECTION_TAG[] = "direction";
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char AMBIENT_COLOR_TAG[] = "ambientColor";
            static constexpr char EXPONENTIAL_ATTENUATION_TAG[] = "exponentialAttenuation";
            static constexpr char PRODUCE_SHADOW_TAG[] = "produceShadow";
    };

}
