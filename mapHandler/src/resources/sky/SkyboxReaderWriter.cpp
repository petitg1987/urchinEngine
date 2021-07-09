#include "SkyboxReaderWriter.h"

namespace urchin {
    std::unique_ptr<Skybox> SkyboxReaderWriter::loadFrom(const XmlChunk* skyChunk, const XmlParser& xmlParser) {
        std::unique_ptr<Skybox> skybox(nullptr);

        auto skyboxChunk = xmlParser.getUniqueChunk(false, SKYBOX_TAG, XmlAttribute(), skyChunk);
        if (skyboxChunk != nullptr) {
            std::vector<std::string> filenames;
            filenames.reserve(6);
            auto texturesChunk = xmlParser.getUniqueChunk(true, TEXTURES_TAG, XmlAttribute(), skyboxChunk.get());
            auto filenameListChunk = xmlParser.getChunks(FILENAME_TAG, XmlAttribute(), texturesChunk.get());
            if (filenameListChunk.size() != 6) {
                throw std::runtime_error("Invalid number of skybox filenames found: " + std::to_string(filenameListChunk.size()));
            }
            for (const auto& filenameChunk : filenameListChunk) {
                filenames.emplace_back(filenameChunk->getStringValue());
            }

            auto offsetYChunk = xmlParser.getUniqueChunk(true, OFFSET_Y_TAG, XmlAttribute(), skyboxChunk.get());

            skybox = std::make_unique<Skybox>(filenames);
            skybox->setOffsetY(offsetYChunk->getFloatValue());
        }
        return skybox;
    }

    void SkyboxReaderWriter::writeOn(XmlChunk* skyChunk, const std::unique_ptr<Skybox>& skybox, XmlWriter& xmlWriter) {
        if (skybox != nullptr) {
            auto skyboxChunk = xmlWriter.createChunk(SKYBOX_TAG, XmlAttribute(), skyChunk);

            auto texturesChunk = xmlWriter.createChunk(TEXTURES_TAG, XmlAttribute(), skyboxChunk.get());
            for (const auto& filename : skybox->getFilenames()) {
                auto filenameChunk = xmlWriter.createChunk(FILENAME_TAG, XmlAttribute(), texturesChunk.get());
                filenameChunk->setStringValue(filename);
            }

            auto offsetYChunk = xmlWriter.createChunk(OFFSET_Y_TAG, XmlAttribute(), skyboxChunk.get());
            offsetYChunk->setFloatValue(skybox->getOffsetY());
        }
    }
}
