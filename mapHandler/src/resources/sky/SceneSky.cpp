#include "SceneSky.h"
#include <resources/sky/SkyboxReaderWriter.h>

namespace urchin {
    SceneSky::SceneSky(Renderer3d* renderer3d) :
            renderer3d(renderer3d) {

    }

    const std::unique_ptr<Skybox>& SceneSky::getSkybox() const {
        return renderer3d->getSkyManager().getSkybox();
    }

    void SceneSky::changeSkybox(std::unique_ptr<Skybox> skybox) {
        renderer3d->getSkyManager().setSkybox(std::move(skybox));
    }

    void SceneSky::loadFrom(const DataChunk* chunk, const DataParser& dataParser) {
        changeSkybox(SkyboxReaderWriter::loadFrom(chunk, dataParser));
    }

    void SceneSky::writeOn(DataChunk* chunk, DataWriter& dataWriter) const {
        SkyboxReaderWriter::writeOn(chunk, getSkybox(), dataWriter);
    }
}
