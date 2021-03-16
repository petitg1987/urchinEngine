#ifndef URCHINENGINE_GENERICRENDERERBUILDER_H
#define URCHINENGINE_GENERICRENDERERBUILDER_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "graphic/render/GenericRenderer.h"
#include "graphic/render/model/DataType.h"
#include "graphic/render/model/DataDimension.h"
#include "graphic/render/model/ShapeType.h"
#include "graphic/render/model/PolygonMode.h"
#include "graphic/render/target/RenderTarget.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/texture/TextureReader.h"

namespace urchin {

    class GenericRendererBuilder {
        public:
            explicit GenericRendererBuilder(std::shared_ptr<RenderTarget>, std::shared_ptr<Shader>, ShapeType);

            const std::shared_ptr<RenderTarget>& getRenderTarget() const;
            const std::shared_ptr<Shader>& getShader() const;
            ShapeType getShapeType() const;

            GenericRendererBuilder* addData(const std::vector<Point2<float>>*);
            GenericRendererBuilder* addData(const std::vector<Point3<float>>*);
            GenericRendererBuilder* addData(const std::vector<Vector3<float>>*);
            const std::vector<GenericRenderer::Data>& getData() const;
            GenericRendererBuilder* indices(const std::vector<unsigned int>*);
            const GenericRenderer::Indices& getIndices() const;

            GenericRendererBuilder* addShaderData(ShaderDataSender&);

            GenericRendererBuilder* enableTransparency();
            bool isTransparencyEnabled() const;

            GenericRendererBuilder* enableDepthTest();
            bool isDepthTestEnabled() const;

            GenericRendererBuilder* disableCullFace();
            bool isCullFaceEnabled() const;

            GenericRendererBuilder* polygonMode(PolygonMode);
            PolygonMode getPolygonMode() const;
            GenericRendererBuilder* outlineSize(float);
            float getOutlineSize() const;

            GenericRendererBuilder* addTextureReader(const TextureReader&);
            const std::vector<TextureReader>& getTextureReaders() const;

            std::unique_ptr<GenericRenderer> build();

        private:
            std::shared_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Shader> shader;
            ShapeType shapeType;
            std::vector<GenericRenderer::Data> data;
            GenericRenderer::Indices pIndices;
            bool transparencyEnabled;
            bool depthTestEnabled;
            bool cullFaceEnabled;
            PolygonMode pPolygonMode;
            float pOutlineSize;
            std::vector<TextureReader> textures;
    };

}

#endif

