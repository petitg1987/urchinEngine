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

    class GenericRendererBuilder {
        public:
            explicit GenericRendererBuilder(std::shared_ptr<RenderTarget>, std::shared_ptr<Shader>, ShapeType);

            const std::shared_ptr<RenderTarget>& getRenderTarget() const;
            const std::shared_ptr<Shader>& getShader() const;
            ShapeType getShapeType() const;

            GenericRendererBuilder* addData(const std::vector<Point2<float>>&);
            GenericRendererBuilder* addData(const std::vector<Point3<float>>&);
            GenericRendererBuilder* addData(const std::vector<Vector3<float>>&);
            const std::vector<DataContainer>& getData() const;
            GenericRendererBuilder* indices(const std::vector<uint32_t>&);
            const std::shared_ptr<IndexContainer>& getIndices() const;

            GenericRendererBuilder* addShaderData(std::size_t, const void*);
            const std::vector<ShaderDataContainer> &getShaderData() const;

            GenericRendererBuilder* addTextureReader(const std::shared_ptr<TextureReader>&);
            GenericRendererBuilder* addTextureReaderArray(const std::vector<std::shared_ptr<TextureReader>>&);
            const std::vector<std::vector<std::shared_ptr<TextureReader>>>& getTextureReaders() const;

            GenericRendererBuilder* enableTransparency();
            bool isTransparencyEnabled() const;

            GenericRendererBuilder* enableDepthOperations();
            bool isDepthOperationsEnabled() const;

            GenericRendererBuilder* disableCullFace();
            bool isCullFaceEnabled() const;

            GenericRendererBuilder* polygonMode(PolygonMode);
            PolygonMode getPolygonMode() const;
            GenericRendererBuilder* outlineSize(float);
            float getOutlineSize() const;

            std::unique_ptr<GenericRenderer> build();

        private:
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
