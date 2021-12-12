#pragma once

#include <Urchin3dEngine.h>

namespace urchin {

    class SkyEntity {
        public:
            SkyEntity();
            ~SkyEntity();

            void setup(Renderer3d*);

            const Skybox* getSkybox() const;
            void setSkybox(std::unique_ptr<Skybox>);

        private:
            Renderer3d* renderer3d;

            std::shared_ptr<Skybox> skybox;
    };

}
