#ifndef URCHINENGINE_WATERREADERWRITER_H
#define URCHINENGINE_WATERREADERWRITER_H

#include <memory>

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

namespace urchin
{

    class WaterReaderWriter
    {
            //XML tags
            #define CENTER_POSITION_TAG "centerPosition"
            #define X_SIZE_TAG "xSize"
            #define Z_SIZE_TAG "zSize"
            #define WATER_COLOR_TAG "waterColor"
            #define NORMAL_FILENAME_TAG "normalFilename"
            #define DUDV_MAP_FILENAME "dudvMapFilename"
            #define S_REPEAT_TAG "sRepeat"
            #define T_REPEAT_TAG "tRepeat"
            #define DENSITY_TAG "density"
            #define GRADIENT_TAG "gradient"

        public:
            Water *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
            void writeOn(std::shared_ptr<XmlChunk>, const Water *, XmlWriter &) const;

        private:
            void loadGeneralPropertiesOn(Water *, std::shared_ptr<XmlChunk>, const XmlParser &) const;
            void writeGeneralPropertiesOn(std::shared_ptr<XmlChunk>, const Water *, XmlWriter &) const;

            void loadUnderWaterProperties(Water *, const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeUnderWaterPropertiesOn(const std::shared_ptr<XmlChunk> &, const Water *, XmlWriter &) const;
    };

}

#endif
