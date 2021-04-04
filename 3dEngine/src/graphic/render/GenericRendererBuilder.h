#ifndef URCHINENGINE_GENERICRENDERERBUILDER_H
#define URCHINENGINE_GENERICRENDERERBUILDER_H

#include <memory>

#include "graphic/render/model/ShapeType.h"
#include "graphic/render/model/PolygonMode.h"
#include "graphic/render/model/DataContainer.h"
#include "graphic/render/model/IndexContainer.h"
#include "graphic/render/target/RenderTarget.h"
#include "graphic/render/shader/model/Shader.h"
#include "graphic/render/shader/model/ShaderDataContainer.h"
#include "graphic/texture/TextureReader.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class GenericRendererBuilder : public std::enable_shared_from_this<GenericRendererBuilder> {
        public:
            static std::shared_ptr<GenericRendererBuilder> create(std::string, std::shared_ptr<RenderTarget>, std::shared_ptr<Shader>, ShapeType);

            const std::string& getName() const;
            const std::shared_ptr<RenderTarget>& getRenderTarget() const;
            const std::shared_ptr<Shader>& getShader() const;
            ShapeType getShapeType() const;

            std::shared_ptr<GenericRendererBuilder> addData(const std::vector<Point2<float>>&);
            std::shared_ptr<GenericRendererBuilder> addData(const std::vector<Point3<float>>&);
            std::shared_ptr<GenericRendererBuilder> addData(const std::vector<Vector3<float>>&);
            const std::vector<DataContainer>& getData() const;
            std::shared_ptr<GenericRendererBuilder> indices(const std::vector<uint32_t>&);
            const std::shared_ptr<IndexContainer>& getIndices() const;

            std::shared_ptr<GenericRendererBuilder> addShaderData(std::size_t, const void*);
            const std::vector<ShaderDataContainer> &getShaderData() const;

            std::shared_ptr<GenericRendererBuilder> addTextureReader(const std::shared_ptr<TextureReader>&);
            std::shared_ptr<GenericRendererBuilder> addTextureReaderArray(const std::vector<std::shared_ptr<TextureReader>>&);
            const std::vector<std::vector<std::shared_ptr<TextureReader>>>& getTextureReaders() const;

            std::shared_ptr<GenericRendererBuilder> enableTransparency();
            bool isTransparencyEnabled() const;

            std::shared_ptr<GenericRendererBuilder> enableDepthOperations();
            bool isDepthOperationsEnabled() const;

            std::shared_ptr<GenericRendererBuilder> disableCullFace();
            bool isCullFaceEnabled() const;

            std::shared_ptr<GenericRendererBuilder> polygonMode(PolygonMode);
            PolygonMode getPolygonMode() const;
            std::shared_ptr<GenericRendererBuilder> outlineSize(float);
            float getOutlineSize() const;

            std::shared_ptr<GenericRenderer> build();

        private:
            GenericRendererBuilder(std::string, std::shared_ptr<RenderTarget>, std::shared_ptr<Shader>, ShapeType);

            std::string name;
            std::shared_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Shader> shader;
            ShapeType shapeType;

            std::vector<DataContainer> data;
            std::shared_ptr<IndexContainer> pIndices;
            std::vector<ShaderDataContainer> shaderData;
            std::vector<std::vector<std::shared_ptr<TextureReader>>> textureReaders;
            bool transparencyEnabled;
            bool depthOperationsEnabled;
            bool cullFaceEnabled;
            PolygonMode pPolygonMode;
            float pOutlineSize;

    };

}

#endif
