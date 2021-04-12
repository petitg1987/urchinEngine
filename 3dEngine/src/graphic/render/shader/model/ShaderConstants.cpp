#include <cstdlib>
#include <cstring>

#include "ShaderConstants.h"

namespace urchin {

    ShaderConstants::ShaderConstants(std::vector<std::size_t> variablesSize, void* data) :
            variablesSize(std::move(variablesSize)) {
        this->data = malloc(sumVariablesSize());
        std::memcpy(this->data, data, sumVariablesSize());
    }

    const std::vector<std::size_t>& ShaderConstants::getVariablesSize() const {
        return variablesSize;
    }

    std::size_t ShaderConstants::sumVariablesSize() const {
        std::size_t dataSize = 0;
        for(auto& variableSize : variablesSize) {
            dataSize += variableSize;
        }
        return dataSize;
    }

    void* ShaderConstants::getData() const {
        return data;
    }

}
