#pragma once

#include <memory>
#include <UrchinCommon.h>

#include <graphics/render/shader/ShaderDataContainer.h>
#include <graphics/render/data/ShapeType.h>
#include <graphics/render/data/PolygonMode.h>
#include <graphics/render/data/DataContainer.h>
#include <graphics/render/data/IndexContainer.h>
#include <graphics/render/data/BlendFunction.h>
#include <graphics/texture/TextureReader.h>
#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class GenericRendererBuilder : public std::enable_shared_from_this<GenericRendererBuilder> {
        public:
            static std::shared_ptr<GenericRendererBuilder> create(std::string, RenderTarget&, const Shader&, ShapeType);

            const std::string& getName() const;
            RenderTarget& getRenderTarget() const;
            const Shader& getShader() const;
            ShapeType getShapeType() const;

            std::shared_ptr<GenericRendererBuilder> addData(const std::vector<Point2<float>>&);
            std::shared_ptr<GenericRendererBuilder> addData(const std::vector<Point3<float>>&);
            std::shared_ptr<GenericRendererBuilder> addData(const std::vector<Vector3<float>>&);
            const std::vector<DataContainer>& getData() const;
            std::shared_ptr<GenericRendererBuilder> instanceData(std::size_t, VariableType, const float*);
            const std::shared_ptr<DataContainer>& getInstanceData() const;
            std::shared_ptr<GenericRendererBuilder> indices(const std::vector<uint32_t>&);
            const std::shared_ptr<IndexContainer>& getIndices() const;

            std::shared_ptr<GenericRendererBuilder> addUniformData(uint32_t, std::size_t, const void*);
            const std::map<uint32_t, ShaderDataContainer>& getUniformData() const;

            std::shared_ptr<GenericRendererBuilder> addUniformTextureReader(uint32_t, const std::shared_ptr<TextureReader>&);
            std::shared_ptr<GenericRendererBuilder> addUniformTextureReaderArray(uint32_t, const std::vector<std::shared_ptr<TextureReader>>&);
            const std::map<uint32_t, std::vector<std::shared_ptr<TextureReader>>>& getUniformTextureReaders() const;

            std::shared_ptr<GenericRendererBuilder> enableTransparency(const std::vector<BlendFunction>&);
            const std::vector<BlendFunction>& getBlendFunctions() const;

            void setScissor(const Vector2<int>&, const Vector2<int>&);
            bool hasCustomScissor() const;
            const Vector2<int>& getScissorOffset() const;
            const Vector2<int>& getScissorSize() const;

            std::shared_ptr<GenericRendererBuilder> enableDepthTest();
            bool isDepthTestEnabled() const;
            std::shared_ptr<GenericRendererBuilder> enableDepthWrite();
            bool isDepthWriteEnabled() const;

            std::shared_ptr<GenericRendererBuilder> disableCullFace();
            bool isCullFaceEnabled() const;

            std::unique_ptr<GenericRenderer> build();

        private:
            GenericRendererBuilder(std::string, RenderTarget&, const Shader&, ShapeType);

            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;
            ShapeType shapeType;

            std::vector<DataContainer> data;
            std::shared_ptr<DataContainer> mInstanceData;
            std::shared_ptr<IndexContainer> mIndices;
            std::map<uint32_t, ShaderDataContainer> uniformData;
            std::map<uint32_t, std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;
            std::vector<BlendFunction> blendFunctions;
            bool customScissor;
            Vector2<int> scissorOffset;
            Vector2<int> scissorSize;
            bool depthTestEnabled;
            bool depthWriteEnabled;
            bool cullFaceEnabled;
    };

}
