#include "SkyboxReaderWriter.h"

namespace urchin
{
    std::vector<std::string> SkyboxReaderWriter::loadFrom(const std::shared_ptr<XmlChunk> &skyboxChunk, const XmlParser &xmlParser) const
    {
        std::vector<std::string> filenames;
        filenames.reserve(6);

        std::vector<std::shared_ptr<XmlChunk>> filenameListChunk = xmlParser.getChunks(FILENAME_TAG, XmlAttribute(), skyboxChunk);
        if(filenameListChunk.size() != 6)
        {
            throw std::runtime_error("Invalid number of skybox filenames found: " + std::to_string(filenameListChunk.size()));
        }

        for (const auto &filenameChunk : filenameListChunk)
        {
            filenames.emplace_back(filenameChunk->getStringValue());
        }

        return filenames;
    }

    void SkyboxReaderWriter::writeOn(const std::shared_ptr<XmlChunk> &skyboxChunk, const Skybox *skybox, XmlWriter &xmlWriter) const
    {
        for(const auto &filename : skybox->getFilenames())
        {
            std::shared_ptr<XmlChunk> filenameChunk = xmlWriter.createChunk(FILENAME_TAG, XmlAttribute(), skyboxChunk);
            filenameChunk->setStringValue(filename);
        }
    }
}
