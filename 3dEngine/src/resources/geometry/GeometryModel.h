#pragma once

#include <UrchinCommon.h>

#include <graphics/api/vulkan/render/shader/model/Shader.h>
#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/GenericRenderer.h>

namespace urchin {

    class GeometryModel {
        public:
            friend class GeometryContainer;
            GeometryModel();
            virtual ~GeometryModel() = default;

            const RenderTarget& getRenderTarget() const;

            Vector3<float> getColor() const;
            void setColor(float, float, float);

            PolygonMode getPolygonMode() const;
            void setPolygonMode(PolygonMode);

            bool isAlwaysVisible() const;
            void setAlwaysVisible(bool);

            bool isCullFaceDisabled() const;
            void disableCullFace();

            void setModelMatrix(const Matrix4<float>&);

        protected:
            void initialize(RenderTarget&);
            void refreshRenderer();

            virtual std::vector<Point3<float>> retrieveVertexArray(std::vector<uint32_t>&) const = 0;
            virtual ShapeType getShapeType() const = 0;

            void prepareRendering(unsigned int, const Matrix4<float>&) const;

        private:
            bool isInitialized;

            RenderTarget* renderTarget;
            std::unique_ptr<GenericRenderer> renderer;

            std::unique_ptr<Shader> shader;
            Matrix4<float> modelMatrix;

            Vector3<float> color;
            PolygonMode polygonMode;
            bool alwaysVisible;
            bool cullFaceDisabled;
    };

}
