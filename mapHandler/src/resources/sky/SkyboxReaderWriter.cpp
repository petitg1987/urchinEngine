#include "SkyboxReaderWriter.h"

namespace urchin {
    std::unique_ptr<Skybox> SkyboxReaderWriter::loadFrom(const DataChunk* skyChunk, const DataParser& dataParser) {
        std::unique_ptr<Skybox> skybox(nullptr);

        auto skyboxChunk = dataParser.getUniqueChunk(false, SKYBOX_TAG, DataAttribute(), skyChunk);
        if (skyboxChunk != nullptr) {
            std::vector<std::string> filenames;
            filenames.reserve(6);
            auto texturesChunk = dataParser.getUniqueChunk(true, TEXTURES_TAG, DataAttribute(), skyboxChunk);
            auto filenameListChunk = dataParser.getChunks(FILENAME_TAG, DataAttribute(), texturesChunk);
            if (filenameListChunk.size() != 6) {
                throw std::runtime_error("Invalid number of skybox filenames found: " + std::to_string(filenameListChunk.size()));
            }
            for (const auto& filenameChunk : filenameListChunk) {
                filenames.emplace_back(filenameChunk->getStringValue());
            }

            auto offsetYChunk = dataParser.getUniqueChunk(true, OFFSET_Y_TAG, DataAttribute(), skyboxChunk);

            skybox = std::make_unique<Skybox>(filenames);
            skybox->setOffsetY(offsetYChunk->getFloatValue());
        }
        return skybox;
    }

    void SkyboxReaderWriter::writeOn(DataChunk& skyChunk, const std::unique_ptr<Skybox>& skybox, DataWriter& dataWriter) {
        if (skybox != nullptr) {
            auto& skyboxChunk = dataWriter.createChunk(SKYBOX_TAG, DataAttribute(), &skyChunk);

            auto& texturesChunk = dataWriter.createChunk(TEXTURES_TAG, DataAttribute(), &skyboxChunk);
            for (const auto& filename : skybox->getFilenames()) {
                auto& filenameChunk = dataWriter.createChunk(FILENAME_TAG, DataAttribute(), &texturesChunk);
                filenameChunk.setStringValue(filename);
            }

            auto& offsetYChunk = dataWriter.createChunk(OFFSET_Y_TAG, DataAttribute(), &skyboxChunk);
            offsetYChunk.setFloatValue(skybox->getOffsetY());
        }
    }
}
