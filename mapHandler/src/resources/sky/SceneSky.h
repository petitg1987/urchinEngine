#ifndef URCHINENGINE_SCENESKY_H
#define URCHINENGINE_SCENESKY_H

#include "Urchin3dEngine.h"

namespace urchin {

    class SceneSky {
        public:
            friend class Map;

            explicit SceneSky(Renderer3d*);

            const std::unique_ptr<Skybox>& getSkybox() const;
            void changeSkybox(std::unique_ptr<Skybox>);

        private:
            void loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            void writeOn(const std::shared_ptr<XmlChunk>&, XmlWriter&) const;

            Renderer3d *renderer3d;
    };

}

#endif
