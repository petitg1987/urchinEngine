#pragma once

#include <Urchin3dEngine.h>

namespace urchin {

    class SkyEntity {
        public:
            friend class Map;

            SkyEntity();
            ~SkyEntity();

            const Skybox* getSkybox() const;
            void setSkybox(std::unique_ptr<Skybox>);

        private:
            void setup(Renderer3d&);

            Renderer3d* renderer3d;

            std::shared_ptr<Skybox> skybox;
    };

}
