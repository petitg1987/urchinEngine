#pragma once

#include <memory>
#include <vector>
#include <Urchin3dEngine.h>

#include <resources/sky/SkyEntity.h>

namespace urchin {

    class SkyEntityReaderWriter {
        public:
            static std::unique_ptr<Skybox> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const SkyEntity&, UdaWriter&);

        private:
            static constexpr char SKYBOX_TAG[] = "skybox";
            static constexpr char TEXTURES_TAG[] = "textures";
            static constexpr char FILENAME_TAG[] = "filename";
            static constexpr char OFFSET_Y_TAG[] = "offsetY";
    };

}
