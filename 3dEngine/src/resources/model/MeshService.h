#pragma once

#include <UrchinCommon.h>

namespace urchin {
    class ConstMesh;
    struct Bone;

    class MeshService {
        public:
            MeshService() = delete;

            static void computeVerticesNormalsTangents(const ConstMesh&, const std::vector<Bone>&, std::vector<Point3<float>>&, std::vector<Vector3<float>>&, std::vector<Vector3<float>>&);
            static void computeNormalsTangents(const ConstMesh&, const std::vector<Point3<float>>&, std::vector<Vector3<float>>&, std::vector<Vector3<float>>&);

        private:
            static void computeVertices(const ConstMesh&, const std::vector<Bone>&, std::vector<Point3<float>>&);
    };

}
