#pragma once

#include <UrchinCommon.h>

#include <graphics/api/GraphicsApi.h>
#include <graphics/render/data/PolygonMode.h>

namespace urchin {

    class GeometryModel {
        public:
            friend class GeometryContainer;
            GeometryModel();
            virtual ~GeometryModel() = default;

            const RenderTarget& getRenderTarget() const;

            Vector3<float> getColor() const;
            void setColor(const Vector3<float>&);

            PolygonMode getPolygonMode() const;
            void setPolygonMode(PolygonMode);
            void setWireframeLineWidth(float);

            bool isAlwaysVisible() const;
            void setAlwaysVisible(bool);

            bool isCullFaceDisabled() const;
            void disableCullFace();

            void setModelMatrix(const Matrix4<float>&);

        protected:
            void initialize(RenderTarget&);
            void refreshRenderer();

            virtual std::vector<Point3<float>> retrieveVertexArray(std::vector<uint32_t>&) const = 0;
            virtual std::vector<LineSegment3D<float>> retrieveWireframeLines() const;
            virtual ShapeType getShapeType() const = 0;

            void prepareRendering(unsigned int, const Matrix4<float>&) const;

        private:
            std::vector<Point3<float>> linesToVertexArray(const std::vector<LineSegment3D<float>>&, std::vector<uint32_t>&, std::vector<Point4<float>>&) const;

            static constexpr uint32_t PVM_MATRIX_UNIFORM_BINDING = 0;
            static constexpr uint32_t WIREFRAME_LINE_WIDTH_UNIFORM_BINDING = 1;
            static constexpr uint32_t COLOR_UNIFORM_BINDING = 2;

            bool isInitialized;

            RenderTarget* renderTarget;
            std::unique_ptr<GenericRenderer> renderer;

            std::unique_ptr<Shader> shader;
            Matrix4<float> modelMatrix;

            Vector3<float> rgbColor;
            PolygonMode polygonMode;
            float wireframeLineWidth;
            bool alwaysVisible;
            bool cullFaceDisabled;
    };

}
