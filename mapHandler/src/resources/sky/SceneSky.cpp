#include "SceneSky.h"
#include "resources/sky/SkyboxReaderWriter.h"

namespace urchin
{
    SceneSky::SceneSky(Renderer3d *renderer3d) :
            renderer3d(renderer3d)
    {

    }

    const std::unique_ptr<Skybox> &SceneSky::getSkybox() const
    {
        return renderer3d->getSkybox();
    }

    void SceneSky::changeSkybox(std::unique_ptr<Skybox> skybox)
    {
        renderer3d->setSkybox(std::move(skybox));
    }

    void SceneSky::loadFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser)
    {
        std::shared_ptr<XmlChunk> skyboxChunk = xmlParser.getUniqueChunk(true, SKYBOX_TAG, XmlAttribute(), chunk);

        changeSkybox(SkyboxReaderWriter().loadFrom(skyboxChunk, xmlParser));
    }

    void SceneSky::writeOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
    {
        std::shared_ptr<XmlChunk> skyboxChunk = xmlWriter.createChunk(SKYBOX_TAG, XmlAttribute(), chunk);

        SkyboxReaderWriter().writeOn(skyboxChunk, getSkybox(), xmlWriter);
    }
}
