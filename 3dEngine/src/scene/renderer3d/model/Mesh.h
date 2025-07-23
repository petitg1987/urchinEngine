#pragma once

#include <UrchinCommon.h>

#include "resources/model/ConstMesh.h"
#include "resources/geometry/sphere/SphereModel.h"
#include "resources/geometry/GeometryContainer.h"

namespace urchin {

    class Mesh {
        public:
            explicit Mesh(const ConstMesh&);

            void updateSkeleton(const std::vector<Bone>&);
            void resetSkeleton();
            void updateVertices(const std::vector<Point3<float>>&);
            void updateUv(const std::vector<Point2<float>>&);
            void updateMaterial(std::shared_ptr<Material>);

            const std::vector<Point3<float>>& getVertices() const;
            const std::vector<Point2<float>>& getUv() const;
            const std::vector<Vector3<float>>& getNormals() const;
            const std::vector<Vector3<float>>& getTangents() const;

            const Material& getMaterial() const;
            const std::shared_ptr<Material>& getMaterialPtr() const;

            void drawBaseBones(GeometryContainer&, const Matrix4<float>&);

        private:
            const ConstMesh& constMesh;

            std::vector<Point3<float>> vertices;
            std::vector<Point2<float>> uv;
            std::vector<Vector3<float>> normals;
            std::vector<Vector3<float>> tangents;

            std::shared_ptr<Material> material;

            std::shared_ptr<SphereModel> boneSphereModels;
    };

}
