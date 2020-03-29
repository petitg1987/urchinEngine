#ifndef URCHINENGINE_SKYBOXREADERWRITER_H
#define URCHINENGINE_SKYBOXREADERWRITER_H

#include <memory>
#include <vector>

#include "Urchin3dEngine.h"

namespace urchin
{

    class SkyboxReaderWriter
    {
        //XML tags
        #define TEXTURES_TAG "textures"
        #define FILENAME_TAG "filename"
        #define OFFSET_Y_TAG "offsetY"

        public:
            std::unique_ptr<Skybox> loadFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeOn(const std::shared_ptr<XmlChunk> &, const std::unique_ptr<Skybox> &, XmlWriter &) const;
    };

}

#endif
