#include "SceneSky.h"
#include <resources/sky/SkyboxReaderWriter.h>

namespace urchin {
    SceneSky::SceneSky(Renderer3d* renderer3d) :
            renderer3d(renderer3d) {

    }

    const Skybox* SceneSky::getSkybox() const {
        return renderer3d->getSkyContainer().getSkybox();
    }

    void SceneSky::changeSkybox(std::unique_ptr<Skybox> skybox) {
        renderer3d->getSkyContainer().setSkybox(std::move(skybox));
    }

    void SceneSky::loadFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        changeSkybox(SkyboxReaderWriter::loadFrom(chunk, udaParser));
    }

    void SceneSky::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        SkyboxReaderWriter::writeOn(chunk, getSkybox(), udaWriter);
    }
}
