#pragma once

#include <string>
#include <vector>
#include <map>
#include <UrchinCommon.h>

#include <resources/material/Material.h>

namespace urchin {

    struct Vertex {
        unsigned int linkedVerticesGroupId;
        int weightStart;
        int weightCount;
    };

    struct Weight {
        int bone; //index of the bone
        float bias; //contribution of the vertex weight
        Point3<float> pos; //coordinates of the vertex weight
    };

    struct Bone {
        std::string name;
        int parent;
        Point3<float> pos;
        Quaternion<float> orient;
    };

    /**
     * Contains all the constant/common data for a mesh.
     * Two identical models can use the instance of this class.
     */
    class ConstMesh {
        public:
            ConstMesh(std::shared_ptr<Material>, const std::vector<Vertex>&, std::vector<Point2<float>>,
                    std::vector<unsigned int>, std::vector<Weight>, const std::vector<Bone>&);

            const Material& getInitialMaterial() const;

            unsigned int getNumberVertices() const;
            const Vertex& getStructVertex(unsigned int) const;
            const std::vector<Point2<float>>& getUvTexture() const;
            const std::vector<unsigned int>& getLinkedVertices(unsigned int) const;

            const std::vector<unsigned int>& getTrianglesIndices() const;

            unsigned int getNumberWeights() const;
            const Weight& getWeight(unsigned int) const;

            unsigned int getNumberBones() const;
            const std::vector<Bone>& getBaseSkeleton() const;
            const Bone& getBaseBone(unsigned int) const;
            const std::vector<Point3<float>>& getBaseVertices() const;
            const std::vector<Vector3<float>>& getBaseNormals() const;
            const std::vector<Vector3<float>>& getBaseTangents() const;

        private:
            std::shared_ptr<Material> initialMaterial;

            std::vector<Vertex> vertices;
            std::vector<Point2<float>> uvTexture;
            std::map<unsigned int, std::vector<unsigned int>> linkedVertices;

            std::vector<unsigned int> trianglesIndices;

            std::vector<Weight> weights;

            //mesh information in bind-pose
            std::vector<Bone> baseSkeleton; //bind-pose skeleton
            std::vector<Point3<float>> baseVertices;
            std::vector<Vector3<float>> baseNormals;
            std::vector<Vector3<float>> baseTangents;
    };
}
