#include <cstring>
#ifdef URCHIN_DEBUG
    #include <cassert>
#endif

#include <graphics/render/shader/ShaderConstants.h>

namespace urchin {

    /**
     * Wrapper for shader constant variables.
     * Variable size and data don't require memory alignment but only 4 bytes scalar can be used as constant ('bool' type must be uint32_t).
     */
    ShaderConstants::ShaderConstants(std::vector<std::size_t> variablesSize, const void* data) :
            variablesSize(std::move(variablesSize)) {
        #ifdef URCHIN_DEBUG
            for (std::size_t variableSize : this->variablesSize) {
                assert(variableSize == 4);
            }
        #endif

        std::size_t sumVariableSize = sumVariablesSize();
        this->data = operator new(sumVariableSize);
        std::memcpy(this->data, data, sumVariableSize);
    }

    ShaderConstants::~ShaderConstants() {
        operator delete(data);
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
