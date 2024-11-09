#include <Urchin3dEngine.h>

#include <map/save/sky/SkyEntityReaderWriter.h>
#include <util/PathUtil.h>

namespace urchin {

    std::unique_ptr<SkyEntity> SkyEntityReaderWriter::load(const UdaChunk* skyChunk, const UdaParser& udaParser) {
        auto skyEntity = std::make_unique<SkyEntity>();

        auto skyboxChunk = udaParser.getFirstChunk(false, SKYBOX_TAG, UdaAttribute(), skyChunk);
        if (skyboxChunk != nullptr) {
            std::vector<std::string> filenames;
            filenames.reserve(6);
            auto texturesChunk = udaParser.getFirstChunk(true, TEXTURES_TAG, UdaAttribute(), skyboxChunk);
            auto filenameListChunk = udaParser.getChunks(FILENAME_TAG, UdaAttribute(), texturesChunk);
            if (filenameListChunk.size() != 6) {
                throw std::runtime_error("Invalid number of skybox filenames found: " + std::to_string(filenameListChunk.size()));
            }
            for (const auto& filenameChunk : filenameListChunk) {
                filenames.emplace_back(filenameChunk->getStringValue());
            }
            auto skybox = std::make_unique<Skybox>(filenames);

            auto offsetYChunk = udaParser.getFirstChunk(true, OFFSET_Y_TAG, UdaAttribute(), skyboxChunk);
            skybox->setOffsetY(offsetYChunk->getFloatValue());

            skyEntity->setSkybox(std::move(skybox));
        }

        return skyEntity;
    }

    void SkyEntityReaderWriter::write(UdaChunk& skyChunk, const SkyEntity& skyEntity, UdaParser& udaParser) {
        if (skyEntity.getSkybox() != nullptr) {
            auto& skyboxChunk = udaParser.createChunk(SKYBOX_TAG, UdaAttribute(), &skyChunk);

            auto& texturesChunk = udaParser.createChunk(TEXTURES_TAG, UdaAttribute(), &skyboxChunk);
            for (const auto& filename : skyEntity.getSkybox()->getFilenames()) {
                std::string relativeFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), filename);
                udaParser.createChunk(FILENAME_TAG, UdaAttribute(), &texturesChunk).setStringValue(relativeFilename);
            }

            udaParser.createChunk(OFFSET_Y_TAG, UdaAttribute(), &skyboxChunk).setFloatValue(skyEntity.getSkybox()->getOffsetY());
        }
    }
}
