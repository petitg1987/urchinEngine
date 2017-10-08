#ifndef URCHINENGINE_TERRAIN_H
#define URCHINENGINE_TERRAIN_H

#include <string>
#include <vector>
#include "UrchinCommon.h"

#include "resources/image/Image.h"

namespace urchin
{

    class Terrain
    {
        public:
            explicit Terrain(const std::string &);
            ~Terrain();

            void onCameraProjectionUpdate(const Matrix4<float> &);

            void display(const Matrix4<float> &) const;

        private:
            std::vector<Point3<float>> buildVertices(const Image *) const;
            std::vector<Vector3<float>> buildNormals(const Image *) const;
            std::vector<unsigned int> buildIndices(const Image *) const;

            std::vector<unsigned int> findTriangleIndices(unsigned int, const Image *) const;

            unsigned int bufferIDs[3], vertexArrayObject;
            enum //buffer IDs indices
            {
                VAO_VERTEX_POSITION = 0,
                VAO_NORMAL,
                VAO_INDEX
            };
            enum //shader input
            {
                SHADER_VERTEX_POSITION = 0,
                SHADER_NORMAL
            };
            unsigned int shader;
            int mProjectionLoc, mViewLoc;

            Matrix4<float> projectionMatrix;

            std::vector<Point3<float>> vertices;
            std::vector<Vector3<float>> normals;
            std::vector<unsigned int> indices;
    };

}

#endif
