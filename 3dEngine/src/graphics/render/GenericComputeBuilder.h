#pragma once

#include <graphics/api/GraphicsApi.h>
#include <graphics/texture/TextureReader.h>
namespace urchin {

    class GenericComputeBuilder : public std::enable_shared_from_this<GenericComputeBuilder> {
        public:
            static std::shared_ptr<GenericComputeBuilder> create(std::string, RenderTarget&, const Shader&, const Vector2<int>&);

            const std::string& getName() const;
            RenderTarget& getRenderTarget() const;
            const Shader& getShader() const;
            const Vector2<int>& getThreadLocalSize() const;

            std::shared_ptr<GenericComputeBuilder> addUniformData(std::size_t, const void*);
            const std::vector<ShaderDataContainer>& getUniformData() const;

            std::shared_ptr<GenericComputeBuilder> addUniformTextureReader(const std::shared_ptr<TextureReader>&);
            const std::vector<std::vector<std::shared_ptr<TextureReader>>>& getUniformTextureReaders() const;

            std::shared_ptr<GenericComputeBuilder> addUniformTextureOutput(const std::shared_ptr<Texture>&);
            const std::vector<std::shared_ptr<Texture>>& getUniformTextureOutputs() const;

            std::unique_ptr<GenericCompute> build();

        private:
            GenericComputeBuilder(std::string, RenderTarget&, const Shader&, const Vector2<int>&);

            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;
            Vector2<int> threadLocalSize;

            std::vector<ShaderDataContainer> uniformData;
            std::vector<std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;
            std::vector<std::shared_ptr<Texture>> uniformTextureOutputs;
    };

}
