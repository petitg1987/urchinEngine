#pragma once

#include <memory>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

namespace urchin {

    class WaterReaderWriter {
            //XML tags
            #define CENTER_POSITION_TAG "centerPosition"
            #define X_SIZE_TAG "xSize"
            #define Z_SIZE_TAG "zSize"
            #define WATER_COLOR_TAG "waterColor"
            #define NORMAL_FILENAME_TAG "normalFilename"
            #define DUDV_MAP_FILENAME_TAG "dudvMapFilename"
            #define WAVE_SPEED_TAG "waveSpeed"
            #define WAVE_STRENGTH_TAG "waveStrength"
            #define S_REPEAT_TAG "sRepeat"
            #define T_REPEAT_TAG "tRepeat"
            #define DENSITY_TAG "density"
            #define GRADIENT_TAG "gradient"

        public:
            Water* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const;
            void writeOn(const std::shared_ptr<XmlChunk>&, const Water*, XmlWriter&) const;

        private:
            static void loadGeneralPropertiesOn(Water*, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeGeneralPropertiesOn(const std::shared_ptr<XmlChunk>&, const Water*, XmlWriter&);

            static void loadWaterSurfaceProperties(Water*, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeWaterSurfacePropertiesOn(const std::shared_ptr<XmlChunk>&, const Water*, XmlWriter&);

            static void loadUnderWaterProperties(Water*, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeUnderWaterPropertiesOn(const std::shared_ptr<XmlChunk>&, const Water*, XmlWriter&);
    };

}
