#pragma once

#include <memory>
#include <vector>

#include <Urchin3dEngine.h>

namespace urchin {

    class SkyboxReaderWriter {
        public:
            static std::unique_ptr<Skybox> loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOn(const std::shared_ptr<XmlChunk>&, const std::unique_ptr<Skybox>&, XmlWriter&);

        private:
            static constexpr char SKYBOX_TAG[] = "skybox";
            static constexpr char TEXTURES_TAG[] = "textures";
            static constexpr char FILENAME_TAG[] = "filename";
            static constexpr char OFFSET_Y_TAG[] = "offsetY";
    };

}
