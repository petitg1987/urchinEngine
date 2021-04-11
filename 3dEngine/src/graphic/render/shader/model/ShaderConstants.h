#ifndef URCHINENGINE_SHADERCONSTANTS_H
#define URCHINENGINE_SHADERCONSTANTS_H

#include <vector>

namespace urchin {

    struct ShaderVarDescription {
        std::size_t dataOffset;
        std::size_t dataSize;
    };

    class ShaderConstants {
        public:
            ShaderConstants(std::vector<ShaderVarDescription>, void*);
            ShaderConstants(const ShaderConstants&) = delete;
            ShaderConstants(ShaderConstants&&) noexcept = delete;

            const std::vector<ShaderVarDescription>& getVariableDescriptions() const;

            std::size_t computeDataSize() const;
            void* getData() const;

        private:
            std::vector<ShaderVarDescription> variableDescriptions;
            void* data;
    };

}

#endif
