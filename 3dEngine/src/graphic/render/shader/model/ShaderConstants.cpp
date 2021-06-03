#include <cstdlib>
#include <cstring>

#include <graphic/render/shader/model/ShaderConstants.h>

namespace urchin {

    /**
     * Wrapper for shader constant variables. Variable size and data doesn't requirement memory alignment but 'bool' type must be of 4 bytes (VkBool32).
     */
    ShaderConstants::ShaderConstants(std::vector<std::size_t> variablesSize, void* data) :
            variablesSize(std::move(variablesSize)) {
        this->data = malloc(sumVariablesSize());
        std::memcpy(this->data, data, sumVariablesSize());
    }

    ShaderConstants::~ShaderConstants() {
        free(data);
    }

    const std::vector<std::size_t>& ShaderConstants::getVariablesSize() const {
        return variablesSize;
    }

    std::size_t ShaderConstants::sumVariablesSize() const {
        std::size_t dataSize = 0;
        for (auto& variableSize : variablesSize) {
            dataSize += variableSize;
        }
        return dataSize;
    }

    void* ShaderConstants::getData() const {
        return data;
    }

}
