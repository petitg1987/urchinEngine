#include <memory>
#include <stdexcept>
#include <stack>
#include <utility>

#include <io/uda/UdaWriter.h>
#include <config/FileSystem.h>

namespace urchin {

    UdaWriter::UdaWriter(std::string filenamePath) :
            filenamePath(std::move(filenamePath)) {

    }

    UdaChunk& UdaWriter::createChunk(const std::string& chunkName, const UdaAttribute& attribute, UdaChunk* parent) {
        std::map<std::string, std::string> attributes;
        if (!attribute.getAttributeName().empty()) {
            attributes.emplace(attribute.getAttributeName(), attribute.getAttributeValue());
        }

        auto newNode = std::make_unique<UdaChunk>(chunkName, "", attributes, parent);
        auto newNodePtr = newNode.get();
        if (!parent) {
            rootNodes.push_back(std::move(newNode));
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

        std::stack<UdaChunk*> stack;
        for (auto it = rootNodes.rbegin(); it != rootNodes.rend(); ++it) {
            stack.push((*it).get());
        }
        while (!stack.empty()) {
            UdaChunk* node = stack.top();
            stack.pop();

            unsigned int indentLevel = computeIndentLevel(*node);
            std::string indent(indentLevel * UdaChunk::INDENT_SPACES, ' ');

            file << indent << buildRawContentLine(*node) << std::endl;

            const auto& children = node->getChildren();
            for (auto it = children.rbegin(); it != children.rend(); ++it) {
                stack.push((*it).get());
            }
        }

        file.close();
    }

    unsigned int UdaWriter::computeIndentLevel(const UdaChunk& udaChunk) const {
        unsigned int indentLevel = 0;
        auto* parentNode = udaChunk.getParent();
        while (parentNode != nullptr) {
            parentNode = parentNode->getParent();
            indentLevel++;
        }
        return indentLevel;
    }

    std::string UdaWriter::buildRawContentLine(const UdaChunk& udaChunk) const {
        std::string rawAttributes;
        if (!udaChunk.getAttributes().empty()) {
            rawAttributes.append(" (");
            auto& attributes = udaChunk.getAttributes();
            for (auto it = attributes.begin(); it != attributes.end(); ++it) {
                if (it != attributes.begin()) {
                    rawAttributes += UdaChunk::ATTRIBUTES_SEPARATOR;
                }
                rawAttributes.append(it->first);
                rawAttributes += UdaChunk::ATTRIBUTES_ASSIGN;
                rawAttributes.append(it->second);
            }
            rawAttributes.append(")");
        }

        std::string rawValue;
        if (!udaChunk.getStringValue().empty()) {
            rawValue = " \"" + udaChunk.getStringValue() + "\"";
        }

        return udaChunk.getName() + rawAttributes + ":" + rawValue;
    }

}
