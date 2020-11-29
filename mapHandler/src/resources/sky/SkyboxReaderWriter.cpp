#include "SkyboxReaderWriter.h"

namespace urchin {
    std::unique_ptr<Skybox> SkyboxReaderWriter::loadFrom(const std::shared_ptr<XmlChunk> &skyChunk, const XmlParser& xmlParser) const {
        std::unique_ptr<Skybox> skybox(nullptr);

        std::shared_ptr<XmlChunk> skyboxChunk = xmlParser.getUniqueChunk(false, SKYBOX_TAG, XmlAttribute(), skyChunk);
        if (skyboxChunk != nullptr) {
            std::vector<std::string> filenames;
            filenames.reserve(6);
            std::shared_ptr<XmlChunk> texturesChunk = xmlParser.getUniqueChunk(true, TEXTURES_TAG, XmlAttribute(), skyboxChunk);
            std::vector<std::shared_ptr<XmlChunk>> filenameListChunk = xmlParser.getChunks(FILENAME_TAG, XmlAttribute(), texturesChunk);
            if (filenameListChunk.size() != 6) {
                throw std::runtime_error("Invalid number of skybox filenames found: " + std::to_string(filenameListChunk.size()));
            }
            for (const auto &filenameChunk : filenameListChunk) {
                filenames.emplace_back(filenameChunk->getStringValue());
            }

            std::shared_ptr<XmlChunk> offsetYChunk = xmlParser.getUniqueChunk(true, OFFSET_Y_TAG, XmlAttribute(), skyboxChunk);

            skybox = std::make_unique<Skybox>(filenames);
            skybox->setOffsetY(offsetYChunk->getFloatValue());
        }
        return skybox;
    }

    void SkyboxReaderWriter::writeOn(const std::shared_ptr<XmlChunk> &skyChunk, const std::unique_ptr<Skybox> &skybox, XmlWriter& xmlWriter) const {
        if (skybox != nullptr) {
            std::shared_ptr<XmlChunk> skyboxChunk = xmlWriter.createChunk(SKYBOX_TAG, XmlAttribute(), skyChunk);

            std::shared_ptr<XmlChunk> texturesChunk = xmlWriter.createChunk(TEXTURES_TAG, XmlAttribute(), skyboxChunk);
            for (const auto &filename : skybox->getFilenames()) {
                std::shared_ptr<XmlChunk> filenameChunk = xmlWriter.createChunk(FILENAME_TAG, XmlAttribute(), texturesChunk);
                filenameChunk->setStringValue(filename);
            }

            std::shared_ptr<XmlChunk> offsetYChunk = xmlWriter.createChunk(OFFSET_Y_TAG, XmlAttribute(), skyboxChunk);
            offsetYChunk->setFloatValue(skybox->getOffsetY());
        }
    }
}
