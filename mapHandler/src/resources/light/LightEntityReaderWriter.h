#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

#include <resources/light/LightEntity.h>

namespace urchin {

    class LightEntityReaderWriter {
        public:
            static std::unique_ptr<LightEntity> loadFrom(const UdaChunk*, const UdaParser&);
            static void writeOn(UdaChunk&, const LightEntity&, UdaWriter&);

        private:
            static std::unique_ptr<Light> buildLightFrom(const UdaChunk*, const UdaParser&);
            static void buildChunkFrom(UdaChunk&, const Light&, UdaWriter&);

            static void loadPropertiesFrom(Light&, const UdaChunk*, const UdaParser&);
            static void writePropertiesOn(UdaChunk&, const Light&, UdaWriter&);

            static void loadFlagsFrom(Light&, const UdaChunk*, const UdaParser&);
            static void writeFlagsOn(UdaChunk&, const Light&, UdaWriter&);

            static constexpr char NAME_ATTR[] = "name";
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
