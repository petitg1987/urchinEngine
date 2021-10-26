#include <cstring>
#include <cassert>

#include <api/render/shader/model/ShaderConstants.h>

namespace urchin {

    /**
     * Wrapper for shader constant variables. Variable size and data doesn't requirement memory alignment but 'bool' type must be of 4 bytes (VkBool32).
     */
    ShaderConstants::ShaderConstants(std::vector<std::size_t> variablesSize, void* data) :
            variablesSize(std::move(variablesSize)) {
        #ifndef NDEBUG
            for (std::size_t variableSize : this->variablesSize) {
                //Others sizes are not handled due to paddings added by the compiler on the structs.
                //Moreover, Vulkan spec states that bool constant should use VkBool32.
                assert(variableSize % 4 == 0);
            }
        #endif

        this->data = ::operator new(sumVariablesSize());
        std::memcpy(this->data, data, sumVariablesSize());
    }

    ShaderConstants::~ShaderConstants() {
        ::operator delete(data);
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
