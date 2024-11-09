#pragma once

#include <memory>

#include <model/SkyEntity.h>

namespace urchin {

    class SkyEntityReaderWriter {
        public:
            static std::unique_ptr<SkyEntity> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const SkyEntity&, UdaParser&);

        private:
            static constexpr char SKYBOX_TAG[] = "skybox";
            static constexpr char TEXTURES_TAG[] = "textures";
            static constexpr char FILENAME_TAG[] = "filename";
            static constexpr char OFFSET_Y_TAG[] = "offsetY";
    };

}
