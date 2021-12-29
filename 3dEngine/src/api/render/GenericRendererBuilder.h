#pragma once

#include <memory>
#include <UrchinCommon.h>

#include <api/render/model/ShapeType.h>
#include <api/render/model/PolygonMode.h>
#include <api/render/model/DataContainer.h>
#include <api/render/model/IndexContainer.h>
#include <api/render/target/RenderTarget.h>
#include <api/render/shader/model/Shader.h>
#include <api/render/shader/model/ShaderDataContainer.h>
#include <api/render/blend/BlendFunction.h>
#include <api/texture/TextureReader.h>
#include <api/render/GenericRenderer.h>

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
            std::shared_ptr<GenericRendererBuilder> addInstanceData(const std::vector<Matrix4<float>>&);
            const std::vector<DataContainer>& getInstanceData() const;
            std::shared_ptr<GenericRendererBuilder> indices(const std::vector<uint32_t>&);
            const std::shared_ptr<IndexContainer>& getIndices() const;

            std::shared_ptr<GenericRendererBuilder> addUniformData(std::size_t, const void*);
            const std::vector<ShaderDataContainer>& getUniformData() const;

            std::shared_ptr<GenericRendererBuilder> addUniformTextureReader(const std::shared_ptr<TextureReader>&);
            std::shared_ptr<GenericRendererBuilder> addUniformTextureReaderArray(const std::vector<std::shared_ptr<TextureReader>>&);
            const std::vector<std::vector<std::shared_ptr<TextureReader>>>& getUniformTextureReaders() const;

            std::shared_ptr<GenericRendererBuilder> enableTransparency(const std::vector<BlendFunction>&);
            const std::vector<BlendFunction>& getBlendFunctions() const;

            std::shared_ptr<GenericRendererBuilder> enableDepthTest();
            bool isDepthTestEnabled() const;
            std::shared_ptr<GenericRendererBuilder> enableDepthWrite();
            bool isDepthWriteEnabled() const;

            std::shared_ptr<GenericRendererBuilder> disableCullFace();
            bool isCullFaceEnabled() const;

            std::shared_ptr<GenericRendererBuilder> polygonMode(PolygonMode);
            PolygonMode getPolygonMode() const;

            std::shared_ptr<GenericRendererBuilder> enableScissor(const Vector2<int>&, const Vector2<unsigned int>&);
            bool isScissorEnabled() const;
            const Vector2<int>& getScissorOffset() const;
            const Vector2<unsigned int>& getScissorSize() const;

            std::unique_ptr<GenericRenderer> build();

        private:
            GenericRendererBuilder(std::string, RenderTarget&, const Shader&, ShapeType);

            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;
            ShapeType shapeType;

            std::vector<DataContainer> data;
            std::vector<DataContainer> instanceData;
            std::shared_ptr<IndexContainer> mIndices;
            std::vector<ShaderDataContainer> uniformData;
            std::vector<std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;
            std::vector<BlendFunction> blendFunctions;
            bool depthTestEnabled;
            bool depthWriteEnabled;
            bool cullFaceEnabled;
            PolygonMode pPolygonMode;
            bool scissorEnabled;
            Vector2<int> scissorOffset;
            Vector2<unsigned int> scissorSize;
    };

}
