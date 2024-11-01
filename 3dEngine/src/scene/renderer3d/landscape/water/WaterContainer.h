#pragma once

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/landscape/water/Water.h>
#include <scene/renderer3d/landscape/fog/FogContainer.h>

namespace urchin {

    class WaterContainer {
        public:
            explicit WaterContainer(RenderTarget&);

            void addWater(const std::shared_ptr<Water>&);
            void removeWater(const Water&);

            void prepareRendering(unsigned int, const Camera&, FogContainer&, float) const;

        private:
            RenderTarget& renderTarget;

            std::vector<std::shared_ptr<Water>> waters;
    };

}
