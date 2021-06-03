#include "SceneSky.h"
#include <resources/sky/SkyboxReaderWriter.h>

namespace urchin {
    SceneSky::SceneSky(Renderer3d* renderer3d) :
            renderer3d(renderer3d) {

    }

    const std::unique_ptr<Skybox>& SceneSky::getSkybox() const {
        return renderer3d->getSkyManager()->getSkybox();
    }

    void SceneSky::changeSkybox(std::unique_ptr<Skybox> skybox) {
        renderer3d->getSkyManager()->setSkybox(std::move(skybox));
    }

    void SceneSky::loadFrom(const std::shared_ptr<XmlChunk>& chunk, const XmlParser& xmlParser) {
        changeSkybox(SkyboxReaderWriter::loadFrom(chunk, xmlParser));
    }

    void SceneSky::writeOn(const std::shared_ptr<XmlChunk>& chunk, XmlWriter& xmlWriter) const {
        SkyboxReaderWriter::writeOn(chunk, getSkybox(), xmlWriter);
    }
}
