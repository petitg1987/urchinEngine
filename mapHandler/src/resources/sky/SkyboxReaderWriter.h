#ifndef URCHINENGINE_SKYBOXREADERWRITER_H
#define URCHINENGINE_SKYBOXREADERWRITER_H

#include <memory>
#include <vector>

#include "Urchin3dEngine.h"

namespace urchin {

    class SkyboxReaderWriter {
        //XML tags
        #define SKYBOX_TAG "skybox"
        #define TEXTURES_TAG "textures"
        #define FILENAME_TAG "filename"
        #define OFFSET_Y_TAG "offsetY"

        public:
            static std::unique_ptr<Skybox> loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOn(const std::shared_ptr<XmlChunk>&, const std::unique_ptr<Skybox>&, XmlWriter&);
    };

}

#endif
