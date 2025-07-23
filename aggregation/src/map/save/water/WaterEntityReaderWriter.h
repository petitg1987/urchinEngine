#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

#include "model/WaterEntity.h"

namespace urchin {

    class WaterEntityReaderWriter {
        public:
            static std::unique_ptr<WaterEntity> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const WaterEntity&, UdaParser&);

        private:
            static void loadGeneralProperties(Water&, const UdaChunk*, const UdaParser&);
            static void writeGeneralProperties(UdaChunk&, const Water&, UdaParser&);

            static void loadWaterSurfaceProperties(Water&, const UdaChunk*, const UdaParser&);
            static void writeWaterSurfaceProperties(UdaChunk&, const Water&, UdaParser&);

            static void loadUnderWaterProperties(Water&, const UdaChunk*, const UdaParser&);
            static void writeUnderWaterProperties(UdaChunk&, const Water&, UdaParser&);

            static constexpr char NAME_ATTR[] = "name";
            static constexpr char CENTER_POSITION_TAG[] = "centerPosition";
            static constexpr char X_SIZE_TAG[] = "xSize";
            static constexpr char Z_SIZE_TAG[] = "zSize";
            static constexpr char WATER_COLOR_TAG[] = "waterColor";
            static constexpr char NORMAL_FILENAME_TAG[] = "normalFilename";
            static constexpr char DUDV_MAP_FILENAME_TAG[] = "dudvMapFilename";
            static constexpr char WAVE_SPEED_TAG[] = "waveSpeed";
            static constexpr char WAVE_STRENGTH_TAG[] = "waveStrength";
            static constexpr char S_REPEAT_TAG[] = "sRepeat";
            static constexpr char T_REPEAT_TAG[] = "tRepeat";
            static constexpr char DENSITY_TAG[] = "density";
            static constexpr char GRADIENT_TAG[] = "gradient";
    };

}
