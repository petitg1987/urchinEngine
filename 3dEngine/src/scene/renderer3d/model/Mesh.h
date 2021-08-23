#pragma once

#include <UrchinCommon.h>

#include <graphic/render/GenericRenderer.h>
#include <graphic/render/target/RenderTarget.h>
#include <resources/model/ConstMesh.h>
#include <resources/geometry/sphere/SphereModel.h>
#include <resources/geometry/GeometryContainer.h>

namespace urchin {

    class Mesh {
        public:
            explicit Mesh(const ConstMesh&);

            void update(const std::vector<Bone>&);

            const std::vector<Point3<float>>& getVertices() const;
            const std::vector<Vector3<float>>& getNormals() const;
            const std::vector<Vector3<float>>& getTangents() const;

            void drawBaseBones(GeometryContainer&, const Matrix4<float>&);

        private:
            const ConstMesh& constMesh;

            std::vector<Point3<float>> vertices;
            std::vector<Vector3<float>> normals;
            std::vector<Vector3<float>> tangents;

            std::shared_ptr<SphereModel> boneSphereModels;
    };

}
