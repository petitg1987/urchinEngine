#pragma once

#include <UrchinCommon.h>

#include <graphic/render/shader/model/Shader.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/GenericRenderer.h>

namespace urchin {

    class GeometryModel {
        public:
            friend class GeometryContainer;
            GeometryModel();
            virtual ~GeometryModel() = default;

            const RenderTarget& getRenderTarget() const;

            Vector4<float> getColor() const;
            void setColor(float, float, float, float alpha = 1.0f);

            PolygonMode getPolygonMode() const;
            void setPolygonMode(PolygonMode);

            bool isTransparencyEnabled() const;
            void enableTransparency();

            bool isAlwaysVisible() const;
            void setAlwaysVisible(bool);

            void setModelMatrix(const Matrix4<float>&);
        protected:
            void initialize(RenderTarget&);
            void onCameraProjectionUpdate(const Matrix4<float>&);
            void refreshRenderer();

            virtual std::vector<Point3<float>> retrieveVertexArray() const = 0;
            virtual ShapeType getShapeType() const = 0;

            void prepareRendering(const Matrix4<float>&) const;

        private:
            bool isInitialized;

            RenderTarget* renderTarget;
            std::unique_ptr<GenericRenderer> renderer;

            std::unique_ptr<Shader> shader;
            mutable struct {
                alignas(16) Matrix4<float> projectionMatrix;
                alignas(16) Matrix4<float> viewModelMatrix;
            } positioningData;

            Matrix4<float> modelMatrix;

            Vector4<float> color;
            PolygonMode polygonMode;
            bool transparencyEnabled;
            bool alwaysVisible;
    };

}
