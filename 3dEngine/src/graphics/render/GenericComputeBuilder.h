#pragma once

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/shader/Shader.h>
#include <graphics/api/vulkan/render/GenericCompute.h>
namespace urchin {

    class GenericComputeBuilder : public std::enable_shared_from_this<GenericComputeBuilder> {
        public:
            static std::shared_ptr<GenericComputeBuilder> create(std::string, RenderTarget&, const Shader&);

            const std::string& getName() const;
            RenderTarget& getRenderTarget() const;
            const Shader& getShader() const;

            std::unique_ptr<GenericCompute> build();

        private:
            GenericComputeBuilder(std::string, RenderTarget&, const Shader&);

            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;
    };

}
