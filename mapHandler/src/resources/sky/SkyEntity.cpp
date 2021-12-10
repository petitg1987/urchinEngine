#include <resources/sky/SkyEntity.h>
#include <resources/sky/SkyboxReaderWriter.h>

namespace urchin {
    SkyEntity::SkyEntity(Renderer3d* renderer3d) :
            renderer3d(renderer3d) {

    }

    const Skybox* SkyEntity::getSkybox() const {
        return renderer3d->getSkyContainer().getSkybox();
    }

    void SkyEntity::changeSkybox(std::unique_ptr<Skybox> skybox) {
        renderer3d->getSkyContainer().setSkybox(std::move(skybox));
    }

    void SkyEntity::loadFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        changeSkybox(SkyboxReaderWriter::loadFrom(chunk, udaParser));
    }

    void SkyEntity::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        SkyboxReaderWriter::writeOn(chunk, getSkybox(), udaWriter);
    }
}
