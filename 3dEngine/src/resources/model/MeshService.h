#pragma once

#include <UrchinCommon.h>

namespace urchin {
    class ConstMesh;
    struct Bone;

    class MeshService {
        public:
            MeshService() = delete;

            static void computeVertices(const ConstMesh&, const std::vector<Bone>&, std::vector<Point3<float>>&);
            static void computeNormalsAndTangents(const ConstMesh&, const std::vector<Bone>&, const std::vector<Point3<float>>&, std::vector<Vector3<float>>&, std::vector<Vector3<float>>&);

        private:
            static bool isAnimated(const ConstMesh&, const std::vector<Bone>&);
    };

}
