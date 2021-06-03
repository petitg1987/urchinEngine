#ifndef URCHINENGINE_MESH_H
#define URCHINENGINE_MESH_H

#include <UrchinCommon.h>

#include <resources/model/ConstMesh.h>
#include <resources/geometry/points/PointsModel.h>
#include <graphic/render/GenericRenderer.h>
#include <graphic/render/target/RenderTarget.h>

namespace urchin {

    class Mesh {
        public:
            explicit Mesh(const ConstMesh*);

            void update(const std::vector<Bone>&);

            const std::vector<Point3<float>>& getVertices() const;
            const std::vector<Vector3<float>>& getNormals() const;
            const std::vector<Vector3<float>>& getTangents() const;

            void drawBaseBones(const std::shared_ptr<RenderTarget>&, const Matrix4<float>&, const Matrix4<float>&);

        private:
            const ConstMesh *const constMesh;

            std::vector<Point3<float>> vertices;
            std::vector<Vector3<float>> normals;
            std::vector<Vector3<float>> tangents;

            std::unique_ptr<PointsModel> pointsModel;
    };

}

#endif
