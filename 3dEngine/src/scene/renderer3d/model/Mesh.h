#pragma once

#include <UrchinCommon.h>

#include <api/render/GenericRenderer.h>
#include <api/render/target/RenderTarget.h>
#include <resources/model/ConstMesh.h>
#include <resources/geometry/sphere/SphereModel.h>
#include <resources/geometry/GeometryContainer.h>

namespace urchin {

    class Mesh {
        public:
            explicit Mesh(const ConstMesh&);

            void update(const std::vector<Bone>&);
            void updateVertices(const std::vector<Point3<float>>&);
            void updateMaterial(const std::shared_ptr<Material>&);

            const std::vector<Point3<float>>& getVertices() const;
            const std::vector<Vector3<float>>& getNormals() const;
            const std::vector<Vector3<float>>& getTangents() const;

            const Material& getMaterial() const;
            const std::shared_ptr<Material>& getMaterialPtr() const;

            void drawBaseBones(GeometryContainer&, const Matrix4<float>&);

        private:
            const ConstMesh& constMesh;

            std::vector<Point3<float>> vertices;
            std::vector<Vector3<float>> normals;
            std::vector<Vector3<float>> tangents;

            std::shared_ptr<Material> material;

            std::shared_ptr<SphereModel> boneSphereModels;
    };

}
