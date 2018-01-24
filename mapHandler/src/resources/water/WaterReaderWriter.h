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
            #define S_REPEAT_TAG "sRepeat"
            #define T_REPEAT_TAG "tRepeat"

        public:
            Water *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
            void writeOn(std::shared_ptr<XmlChunk>, const Water *, XmlWriter &) const;

        private:
            Water *buildWaterFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
            void buildChunkFrom(std::shared_ptr<XmlChunk>, const Water *, XmlWriter &) const;

            void loadPropertiesOn(Water *, std::shared_ptr<XmlChunk>, const XmlParser &) const;
            void writePropertiesOn(std::shared_ptr<XmlChunk>, const Water *, XmlWriter &) const;
    };

}

#endif
