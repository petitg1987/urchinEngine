#ifndef URCHINENGINE_GEOMETRYMODEL_H
#define URCHINENGINE_GEOMETRYMODEL_H

#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"

namespace urchin {

    class GeometryModel {
        public:
            GeometryModel();
            virtual ~GeometryModel();

            enum PolygonMode
            {
                WIREFRAME,
                FILL
            };

            void onCameraProjectionUpdate(const Matrix4<float> &);

            Vector4<float> getColor() const;
            void setColor(float, float, float, float alpha = 1.0f);

            PolygonMode getPolygonMode() const;
            void setPolygonMode(PolygonMode);
            void setLineSize(float);

            bool isTransparencyEnabled() const;
            void enableTransparency();

            bool isAlwaysVisible() const;
            void setAlwaysVisible(bool);

            void display(const Matrix4<float> &) const;

        protected:
            virtual Matrix4<float> retrieveModelMatrix() const = 0;
            virtual std::vector<Point3<float>> retrieveVertexArray() const = 0;

            void initialize();

            virtual void drawGeometry() const = 0;

        private:
            unsigned int bufferIDs[1], vertexArrayObject;
            enum //buffer IDs indices
            {
                VAO_VERTEX_POSITION = 0,
            };
            enum //shader input
            {
                SHADER_VERTEX_POSITION = 0,
            };
            std::unique_ptr<Shader> shader;
            int mProjectionLoc, mViewLoc, colorLoc;

            Matrix4<float> projectionMatrix;
            Matrix4<float> modelMatrix;

            Vector4<float> color;

            PolygonMode polygonMode;
            float lineSize;

            bool transparencyEnabled;
            bool alwaysVisible;
    };

}

#endif
