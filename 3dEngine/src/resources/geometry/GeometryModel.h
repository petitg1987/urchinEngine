#ifndef URCHINENGINE_GEOMETRYMODEL_H
#define URCHINENGINE_GEOMETRYMODEL_H

#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/target/RenderTarget.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class GeometryModel {
        public:
            GeometryModel();
            virtual ~GeometryModel() = default;

            void onCameraProjectionUpdate(const Matrix4<float> &);

            Vector4<float> getColor() const;
            void setColor(float, float, float, float alpha = 1.0f);

            PolygonMode getPolygonMode() const;
            void setPolygonMode(PolygonMode);
            void setOutlineSize(float);

            bool isTransparencyEnabled() const;
            void enableTransparency();

            bool isAlwaysVisible() const;
            void setAlwaysVisible(bool);

            void display(const RenderTarget *, const Matrix4<float> &) const;

        protected:
            void initialize();
            void refreshRenderer();

            virtual Matrix4<float> retrieveModelMatrix() const = 0;
            virtual std::vector<Point3<float>> retrieveVertexArray() const = 0;

            virtual ShapeType getShapeType() const = 0;

        private:
            std::unique_ptr<GenericRenderer> renderer;

            std::unique_ptr<Shader> shader;
            ShaderVar mProjectionShaderVar, mViewShaderVar, colorShaderVar;

            Matrix4<float> projectionMatrix;
            Matrix4<float> modelMatrix;

            Vector4<float> color;
            PolygonMode polygonMode;
            float outlineSize;
            bool transparencyEnabled;
            bool alwaysVisible;
    };

}

#endif
