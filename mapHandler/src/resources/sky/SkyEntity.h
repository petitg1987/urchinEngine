#pragma once

#include <Urchin3dEngine.h>

namespace urchin {

    class SkyEntity {
        public:
            explicit SkyEntity(Renderer3d*);

            const Skybox* getSkybox() const;
            void changeSkybox(std::unique_ptr<Skybox>);

        private:
            Renderer3d* renderer3d;
    };

}
