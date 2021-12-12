#include <resources/sky/SkyEntityReaderWriter.h>

namespace urchin {

    std::unique_ptr<SkyEntity> SkyEntityReaderWriter::load(const UdaChunk* skyChunk, const UdaParser& udaParser) {
        std::unique_ptr<SkyEntity> skyEntity = std::make_unique<SkyEntity>();

        auto skyboxChunk = udaParser.getUniqueChunk(false, SKYBOX_TAG, UdaAttribute(), skyChunk);
        if (skyboxChunk != nullptr) {
            std::vector<std::string> filenames;
            filenames.reserve(6);
            auto texturesChunk = udaParser.getUniqueChunk(true, TEXTURES_TAG, UdaAttribute(), skyboxChunk);
            auto filenameListChunk = udaParser.getChunks(FILENAME_TAG, UdaAttribute(), texturesChunk);
            if (filenameListChunk.size() != 6) {
                throw std::runtime_error("Invalid number of skybox filenames found: " + std::to_string(filenameListChunk.size()));
            }
            for (const auto& filenameChunk : filenameListChunk) {
                filenames.emplace_back(filenameChunk->getStringValue());
            }
            std::unique_ptr<Skybox> skybox = std::make_unique<Skybox>(filenames);

            auto offsetYChunk = udaParser.getUniqueChunk(true, OFFSET_Y_TAG, UdaAttribute(), skyboxChunk);
            skybox->setOffsetY(offsetYChunk->getFloatValue());

            skyEntity->setSkybox(std::move(skybox));
        }

        return skyEntity;
    }

    void SkyEntityReaderWriter::write(UdaChunk& skyChunk, const SkyEntity& skyEntity, UdaWriter& udaWriter) {
        if (skyEntity.getSkybox() != nullptr) {
            auto& skyboxChunk = udaWriter.createChunk(SKYBOX_TAG, UdaAttribute(), &skyChunk);

            auto& texturesChunk = udaWriter.createChunk(TEXTURES_TAG, UdaAttribute(), &skyboxChunk);
            for (const auto& filename : skyEntity.getSkybox()->getFilenames()) {
                auto& filenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &texturesChunk);
                filenameChunk.setStringValue(filename);
            }

            auto& offsetYChunk = udaWriter.createChunk(OFFSET_Y_TAG, UdaAttribute(), &skyboxChunk);
            offsetYChunk.setFloatValue(skyEntity.getSkybox()->getOffsetY());
        }
    }
}
