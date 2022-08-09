#include <limits>

#include <resources/model/MeshService.h>
#include <resources/model/ConstAnimation.h>

namespace urchin {

    /**
     * @param vertices [out] Computed vertices based on the skeleton
     */
    void MeshService::computeVertices(const ConstMesh& constMesh, const std::vector<Bone>& skeleton, std::vector<Point3<float>>& vertices) {
        //setup vertices
        vertices.clear();
        vertices.resize(constMesh.getNumberVertices(), Point3<float>(0.0f, 0.0f, 0.0f));
        for (unsigned int i = 0; i < constMesh.getNumberVertices(); ++i) {
            //calculate final vertex to draw with weights
            for (int j = 0; j < constMesh.getStructVertex(i).weightCount; ++j) {
                const Weight& weight = constMesh.getWeight((unsigned int)(constMesh.getStructVertex(i).weightStart + j));
                const Bone& bone = skeleton[weight.boneIndex];

                //calculate transformed vertex for this weight
                Point3<float> wv = bone.orient.rotatePoint(weight.pos);

                //the sum of all weight->bias should be 1.0
                vertices[i].X += (bone.pos.X + wv.X) * weight.bias;
                vertices[i].Y += (bone.pos.Y + wv.Y) * weight.bias;
                vertices[i].Z += (bone.pos.Z + wv.Z) * weight.bias;
            }
        }
    }

    /**
     * @param normals [out] Computed weighted normals for vertices
     * @param tangents [out] Computed tangents for vertices
     */
    void MeshService::computeNormalsAndTangents(const ConstMesh& constMesh, const std::vector<Point3<float>>& vertices, std::vector<Vector3<float>>& normals,
                                                std::vector<Vector3<float>>& tangents) {
        //compute weighted normals
        static std::vector<Vector3<float>> vertexNormals;
        vertexNormals.clear();
        vertexNormals.resize(constMesh.getNumberVertices(), Vector3<float>(0.0f, 0.0f, 0.0f));

        std::size_t numTrianglesIndices = constMesh.getTrianglesIndices().size();
        assert(numTrianglesIndices % 3 == 0);
        for (std::size_t triIndices = 0; triIndices < numTrianglesIndices; triIndices += 3) {
            unsigned int triIndex1 = constMesh.getTrianglesIndices()[triIndices + 0];
            unsigned int triIndex2 = constMesh.getTrianglesIndices()[triIndices + 1];
            unsigned int triIndex3 = constMesh.getTrianglesIndices()[triIndices + 2];

            const Point3<float>& vert1 = vertices[triIndex1];
            const Point3<float>& vert2 = vertices[triIndex2];
            const Point3<float>& vert3 = vertices[triIndex3];

            Vector3<float> vert12 = vert1.vector(vert2);
            Vector3<float> vert13 = vert1.vector(vert3);

            float vert12Length = vert12.length();
            float vert13Length = vert13.length();
            float vert23Length = vert2.vector(vert3).length();

            Vector3<float> normal = vert13.crossProduct(vert12);
            float normalLength = normal.length();
            Vector3<float> normalizedNormal = normal / normalLength;

            //see https://stackoverflow.com/questions/18519586/calculate-normal-per-vertex-opengl
            //note: sinAlpha must be clamped between -0.999999f and 0.999999f in case of std::asin usage
            float sinAlpha1 = normalLength / (vert12Length * vert13Length);
            vertexNormals[triIndex1] += normalizedNormal * MathFunction::approximateAsin(sinAlpha1);
            float sinAlpha2 = normalLength / (vert23Length * vert12Length);
            vertexNormals[triIndex2] += normalizedNormal * MathFunction::approximateAsin(sinAlpha2);
            float sinAlpha3 = normalLength / (vert13Length * vert23Length);
            vertexNormals[triIndex3] += normalizedNormal * MathFunction::approximateAsin(sinAlpha3);
        }

        //sum weighted normals of same vertex
        normals.clear();
        normals.resize(constMesh.getNumberVertices(), Vector3<float>(0.0f, 0.0f, 0.0f));
        for (unsigned int vertexIndex = 0; vertexIndex < constMesh.getNumberVertices(); ++vertexIndex) {
            unsigned int linkedVerticesGroupId = constMesh.getStructVertex(vertexIndex).linkedVerticesGroupId;
            for (unsigned int linkedVertex : constMesh.getLinkedVertices(linkedVerticesGroupId)) {
                normals[vertexIndex] += vertexNormals[linkedVertex];
            }
        }

        tangents.resize(constMesh.getNumberVertices());
        for (unsigned int vertexIndex = 0; vertexIndex < constMesh.getNumberVertices(); ++vertexIndex) {
            //normal normalization
            normals[vertexIndex] = normals[vertexIndex].normalize();

            //computes tangent
            Vector3<float> c1 = normals[vertexIndex].crossProduct(Vector3<float>(0.0f, 0.0f, 1.0f));
            Vector3<float> c2 = normals[vertexIndex].crossProduct(Vector3<float>(0.0f, 1.0f, 0.0f));
            if (c1.squareLength() > c2.squareLength()) {
                tangents[vertexIndex] = c1.normalize();
            } else {
                tangents[vertexIndex] = c2.normalize();
            }
        }
    }
}
