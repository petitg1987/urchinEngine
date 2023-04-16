#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

#include <model/LightEntity.h>

namespace urchin {

    class LightEntityReaderWriter {
        public:
            static std::unique_ptr<LightEntity> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const LightEntity&, UdaParser&);

        private:
            static std::unique_ptr<Light> buildLight(const UdaChunk*, const UdaParser&);
            static void writeLightChunk(UdaChunk&, const Light&, UdaParser&);

            static void loadProperties(Light&, const UdaChunk*, const UdaParser&);
            static void writeProperties(UdaChunk&, const Light&, UdaParser&);

            static void loadFlags(Light&, const UdaChunk*, const UdaParser&);
            static void writeFlags(UdaChunk&, const Light&, UdaParser&);

            static constexpr char NAME_ATTR[] = "name";
            static constexpr char TYPE_ATTR[] = "type";
            static constexpr char SUN_VALUE[] = "SUN";
            static constexpr char OMNIDIRECTIONAL_VALUE[] = "OMNIDIRECTIONAL";
            static constexpr char SPOT_VALUE[] = "SPOT";
            static constexpr char DIRECTION_TAG[] = "direction";
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char INNER_ANGLE_TAG[] = "innerAngle";
            static constexpr char OUTER_ANGLE_TAG[] = "outerAngle";
            static constexpr char LIGHT_COLOR_TAG[] = "lightColor";
            static constexpr char EXPONENTIAL_ATTENUATION_TAG[] = "exponentialAttenuation";
            static constexpr char PBR_ENABLED_TAG[] = "pbrEnabled";
            static constexpr char PRODUCE_SHADOW_TAG[] = "produceShadow";
    };

}
