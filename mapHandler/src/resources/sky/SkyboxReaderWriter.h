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
        #define FILENAME_TAG "filename"

        public:
            std::vector<std::string> loadFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeOn(const std::shared_ptr<XmlChunk> &, const std::unique_ptr<Skybox> &, XmlWriter &) const;
    };

}

#endif
