#include <stdexcept>

#include <io/data/DataWriter.h>
#include <config/FileSystem.h>

namespace urchin {

    DataWriter::DataWriter(const std::string& filename) :
            filenamePath(FileSystem::instance()->getResourcesDirectory() + filename) {

    }

    std::unique_ptr<DataChunk> DataWriter::createChunk(const std::string& chunkName, const DataAttribute& attribute, const DataChunk* parent) {
        std::map<std::string, std::string> attributes;
        if (!attribute.getAttributeName().empty()) {
            attributes.emplace(attribute.getAttributeName(), attribute.getAttributeValue());
        }

        DataContentLine* nodeParent = parent ? &parent->chunk : nullptr;
        auto newNode = std::make_unique<DataContentLine>(chunkName, "", attributes, nodeParent);
        auto& newNodeRef = *newNode;
        if (!parent) {
            root = std::move(newNode);
        } else {
            parent->chunk.addChild(std::move(newNode));
        }

        return std::unique_ptr<DataChunk>(new DataChunk(newNodeRef));
    }

    void DataWriter::saveInFile() {
        std::ofstream file;
        file.open (filenamePath);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + filenamePath);
        }

        std::stack<DataContentLine*> stack;
        stack.push(root.get());
        while (!stack.empty()) {
            DataContentLine* node = stack.top();
            stack.pop();

            unsigned int indentLevel = computeIndentLevel(*node);
            std::string indent(indentLevel * DataChunk::INDENT_SPACES, ' ');

            file << indent << DataContentLine::toRawContentLine(*node) << std::endl;

            const auto& children = node->getChildren();
            for (auto it = children.rbegin(); it != children.rend(); ++it) {
                stack.push((*it).get());
            }
        }

        file.close();
    }

    unsigned int DataWriter::computeIndentLevel(const DataContentLine& dataContentLine) const {
        unsigned int indentLevel = 0;
        auto* parentNode = dataContentLine.getParent();
        while (parentNode != nullptr) {
            parentNode = parentNode->getParent();
            indentLevel++;
        }
        return indentLevel;
    }

}
