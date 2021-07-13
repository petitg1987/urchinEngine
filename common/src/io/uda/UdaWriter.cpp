#include <memory>
#include <stdexcept>
#include <stack>

#include <io/uda/UdaWriter.h>
#include <config/FileSystem.h>

namespace urchin {

    UdaWriter::UdaWriter(const std::string& filename) :
            filenamePath(FileSystem::instance()->getResourcesDirectory() + filename) {

    }

    DataChunk& UdaWriter::createChunk(const std::string& chunkName, const UdaAttribute& attribute, DataChunk* parent) {
        std::map<std::string, std::string> attributes;
        if (!attribute.getAttributeName().empty()) {
            attributes.emplace(attribute.getAttributeName(), attribute.getAttributeValue());
        }

        auto newNode = std::make_unique<DataChunk>(chunkName, "", attributes, parent);
        auto newNodePtr = newNode.get();
        if (!parent) {
            root = std::move(newNode);
        } else {
            parent->addChild(std::move(newNode));
        }

        return *newNodePtr;
    }

    void UdaWriter::saveInFile() {
        std::ofstream file;
        file.open (filenamePath);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + filenamePath);
        }

        std::stack<DataChunk*> stack;
        stack.push(root.get());
        while (!stack.empty()) {
            DataChunk* node = stack.top();
            stack.pop();

            unsigned int indentLevel = computeIndentLevel(*node);
            std::string indent(indentLevel * DataChunk::INDENT_SPACES, ' ');

            file << indent << buildRawContentLine(*node) << std::endl;

            const auto& children = node->getChildren();
            for (auto it = children.rbegin(); it != children.rend(); ++it) {
                stack.push((*it).get());
            }
        }

        file.close();
    }

    unsigned int UdaWriter::computeIndentLevel(const DataChunk& dataChunk) const {
        unsigned int indentLevel = 0;
        auto* parentNode = dataChunk.getParent();
        while (parentNode != nullptr) {
            parentNode = parentNode->getParent();
            indentLevel++;
        }
        return indentLevel;
    }

    std::string UdaWriter::buildRawContentLine(const DataChunk& dataChunk) const {
        std::string rawAttributes;
        if (!dataChunk.getAttributes().empty()) {
            rawAttributes.append(" (");
            auto& attributes = dataChunk.getAttributes();
            for (auto it = attributes.begin(); it != attributes.end(); ++it) {
                if (it != attributes.begin()) {
                    rawAttributes += DataChunk::ATTRIBUTES_SEPARATOR;
                }
                rawAttributes.append(it->first);
                rawAttributes += DataChunk::ATTRIBUTES_ASSIGN;
                rawAttributes.append(it->second);
            }
            rawAttributes.append(")");
        }

        std::string rawValue;
        if (!dataChunk.getStringValue().empty()) {
            rawValue = " \"" + dataChunk.getStringValue() + "\"";
        }

        return dataChunk.getName() + rawAttributes + ":" + rawValue;
    }

}
