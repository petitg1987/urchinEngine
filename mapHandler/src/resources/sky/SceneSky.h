#pragma once

#include <Urchin3dEngine.h>

namespace urchin {

    class SceneSky {
        public:
            friend class Map;

            explicit SceneSky(Renderer3d*);

            const std::unique_ptr<Skybox>& getSkybox() const;
            void changeSkybox(std::unique_ptr<Skybox>);

        private:
            void loadFrom(const UdaChunk*, const DataParser&);
            void writeOn(UdaChunk&, UdaWriter&) const;

            Renderer3d* renderer3d;
    };

}
