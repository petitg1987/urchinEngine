#include <limits>

#include "resources/model/MeshService.h"
#include "resources/model/ConstAnimation.h"

namespace urchin {

    MeshService::MeshService() : Singleton<MeshService>() {

    }

    /**
     * @param vertices [out] Computed vertices based on the skeleton
     */
    void MeshService::computeVertices(const ConstMesh *const constMesh, const std::vector<Bone> &skeleton, std::vector<Point3<float>> &vertices) {
        //setup vertices
        vertices.resize(constMesh->getNumberVertices());
        for (unsigned int i = 0; i < constMesh->getNumberVertices(); ++i) {
            vertices[i].setNull();

            //calculate final vertex to draw with weights
            for (int j=0;j<constMesh->getStructVertex(i).weightCount;++j) {
                const Weight *weight = &constMesh->getWeight(static_cast<unsigned int>(constMesh->getStructVertex(i).weightStart + j));
                const Bone &bone = skeleton[weight->bone];

                //calculate transformed vertex for this weight
                Point3<float> wv = bone.orient.rotatePoint(weight->pos);

                //the sum of all weight->bias should be 1.0
                vertices[i].X += (bone.pos.X + wv.X) * weight->bias;
                vertices[i].Y += (bone.pos.Y + wv.Y) * weight->bias;
                vertices[i].Z += (bone.pos.Z + wv.Z) * weight->bias;
            }
        }
    }

    /**
     * @param normals [out] Computed weighted normals for vertices
     * @param tangents [out] Computed tangents for vertices
     */
    void MeshService::computeNormalsAndTangents(const ConstMesh *const constMesh, const std::vector<Point3<float>> &vertices, std::vector<Vector3<float>> &normals,
                                                std::vector<Vector3<float>> &tangents) {
        //compute weighted normals
        static std::vector<Vector3<float>> vertexNormals;
        vertexNormals.clear();
        vertexNormals.resize(constMesh->getNumberVertices(), Vector3<float>(0.0f, 0.0f, 0.0f));

        for (unsigned int triIndex=0; triIndex<constMesh->getNumberTriangles(); ++triIndex) {
            const Triangle &tri = constMesh->getTriangle(triIndex);
            for (unsigned int triVertexIndex=0; triVertexIndex<3; ++triVertexIndex) {
                auto vertexIndex = tri.index[triVertexIndex];

                Vector3<float> weightedNormal = computeWeightedVertexNormal(tri, triVertexIndex, vertices);
                vertexNormals[vertexIndex] += weightedNormal;
            }
        }

        //sum weighted normals of same vertex
        normals.resize(constMesh->getNumberVertices());
        for (unsigned int vertexIndex = 0; vertexIndex < constMesh->getNumberVertices(); ++vertexIndex) {
            normals[vertexIndex].setNull();

            unsigned int linkedVerticesGroupId = constMesh->getStructVertex(vertexIndex).linkedVerticesGroupId;
            for (unsigned int linkedVertex : constMesh->getLinkedVertices(linkedVerticesGroupId)) {
                normals[vertexIndex] += vertexNormals[linkedVertex];
            }
        }

        tangents.resize(constMesh->getNumberVertices());
        for (unsigned int vertexIndex = 0; vertexIndex < constMesh->getNumberVertices(); ++vertexIndex) {
            //normal normalization
            normals[vertexIndex] = normals[vertexIndex].normalize();

            //computes tangent
            const Vector3<float> &c1 = normals[vertexIndex].crossProduct(Vector3<float>(0.0f, 0.0f, 1.0f));
            const Vector3<float> &c2 = normals[vertexIndex].crossProduct(Vector3<float>(0.0f, 1.0f, 0.0f));
            if (c1.squareLength() > c2.squareLength()) {
                tangents[vertexIndex] = c1.normalize();
            } else {
                tangents[vertexIndex] = c2.normalize();
            }
        }
    }

    int MeshService::indexOfVertexInTriangle(const Triangle &triangle, unsigned int vertexIndex, const ConstMesh *const constMesh) {
        for (int i=0; i<3; ++i) {
            auto triVertexIndex = triangle.index[i];

            //classic
            if (triVertexIndex==vertexIndex) {
                return i;
            }

            //by group id
            unsigned int linkedVerticesGroupId = constMesh->getStructVertex(triVertexIndex).linkedVerticesGroupId;
            for (unsigned int linkedVertexIndex : constMesh->getLinkedVertices(linkedVerticesGroupId)) {
                if (linkedVertexIndex == vertexIndex) {
                    return i;
                }
            }
        }

        return -1;
    }

    Vector3<float> MeshService::computeWeightedVertexNormal(const Triangle &triangle, unsigned int vertexIndex, const std::vector<Point3<float>> &vertices) {
        //see https://stackoverflow.com/questions/18519586/calculate-normal-per-vertex-opengl
        Point3<float> a = vertices[triangle.index[vertexIndex]];
        Point3<float> b = vertices[triangle.index[(vertexIndex+1)%3]];
        Point3<float> c = vertices[triangle.index[(vertexIndex+2)%3]];

        Vector3<float> ab = a.vector(b);
        Vector3<float> ac = a.vector(c);

        Vector3<float> normal = ac.crossProduct(ab);
        float sinAlpha = normal.length() / (ab.length() * ac.length());
        sinAlpha = MathAlgorithm::clamp(sinAlpha, -1.0f + std::numeric_limits<float>::epsilon(), 1.0f - std::numeric_limits<float>::epsilon()); //because of rounding error

        return normal.normalize() * std::asin(sinAlpha);
    }
}
