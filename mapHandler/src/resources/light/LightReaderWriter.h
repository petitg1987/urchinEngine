#pragma once

#include <memory>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

namespace urchin {

    class LightReaderWriter {
        public:
            static Light* loadFrom(const UdaChunk*, const UdaParser&);
            static void writeOn(UdaChunk&, const Light*, UdaWriter&);

        private:
            static Light* buildLightFrom(const UdaChunk*, const UdaParser&);
            static void buildChunkFrom(UdaChunk&, const Light*, UdaWriter&);

            static void loadPropertiesFrom(Light*, const UdaChunk*, const UdaParser&);
            static void writePropertiesOn(UdaChunk&, const Light*, UdaWriter&);

            static void loadFlagsFrom(Light*, const UdaChunk*, const UdaParser&);
            static void writeFlagsOn(UdaChunk&, const Light*, UdaWriter&);

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
