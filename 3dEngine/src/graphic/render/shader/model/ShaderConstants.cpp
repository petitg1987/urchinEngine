#include <cstdlib>
#include <cstring>

#include "ShaderConstants.h"

namespace urchin {

    ShaderConstants::ShaderConstants(std::vector<ShaderVarDescription> variableDescriptions, void* data) :
            variableDescriptions(std::move(variableDescriptions)) {
        this->data = malloc(computeDataSize());
        std::memcpy(this->data, data, computeDataSize());
    }

    const std::vector<ShaderVarDescription>& ShaderConstants::getVariableDescriptions() const {
        return variableDescriptions;
    }

    std::size_t ShaderConstants::computeDataSize() const {
        std::size_t dataSize = 0;
        for(auto& variableDescription : variableDescriptions) {
            dataSize += variableDescription.dataSize;
        }
        return dataSize;
    }

    void* ShaderConstants::getData() const {
        return data;
    }

}
