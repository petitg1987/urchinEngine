#include <resources/sky/SkyboxReaderWriter.h>

namespace urchin {
    std::unique_ptr<Skybox> SkyboxReaderWriter::loadFrom(const UdaChunk* skyChunk, const UdaParser& udaParser) {
        std::unique_ptr<Skybox> skybox(nullptr);

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
            skybox = std::make_unique<Skybox>(filenames);

            auto offsetYChunk = udaParser.getUniqueChunk(true, OFFSET_Y_TAG, UdaAttribute(), skyboxChunk);
            skybox->setOffsetY(offsetYChunk->getFloatValue());
        }
        return skybox;
    }

    void SkyboxReaderWriter::writeOn(UdaChunk& skyChunk, const Skybox* skybox, UdaWriter& udaWriter) {
        if (skybox != nullptr) {
            auto& skyboxChunk = udaWriter.createChunk(SKYBOX_TAG, UdaAttribute(), &skyChunk);

            auto& texturesChunk = udaWriter.createChunk(TEXTURES_TAG, UdaAttribute(), &skyboxChunk);
            for (const auto& filename : skybox->getFilenames()) {
                auto& filenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &texturesChunk);
                filenameChunk.setStringValue(filename);
            }

            auto& offsetYChunk = udaWriter.createChunk(OFFSET_Y_TAG, UdaAttribute(), &skyboxChunk);
            offsetYChunk.setFloatValue(skybox->getOffsetY());
        }
    }
}
