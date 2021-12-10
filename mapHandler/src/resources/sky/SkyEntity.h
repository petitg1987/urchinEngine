#pragma once

#include <Urchin3dEngine.h>

namespace urchin {

    class SkyEntity {
        public:
            friend class Map;

            explicit SkyEntity(Renderer3d*);

            const Skybox* getSkybox() const;
            void changeSkybox(std::unique_ptr<Skybox>);

        private:
            void loadFrom(const UdaChunk*, const UdaParser&);
            void writeOn(UdaChunk&, UdaWriter&) const;

            Renderer3d* renderer3d;
    };

}
