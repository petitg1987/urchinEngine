#ifndef URCHINENGINE_MESHSERVICE_H
#define URCHINENGINE_MESHSERVICE_H

#include "UrchinCommon.h"

namespace urchin {
    class ConstMesh;
    struct Bone;

    class MeshService {
        public:
            MeshService() = delete;

            static void computeVertices(const ConstMesh*, const std::vector<Bone>&, std::vector<Point3<float>>&);
            static void computeNormalsAndTangents(const ConstMesh*, const std::vector<Point3<float>>&, std::vector<Vector3<float>>&, std::vector<Vector3<float>>&);

        private:
            static Vector3<float> computeWeightedVertexNormal(unsigned int, unsigned int, unsigned int, const std::vector<Point3<float>>&);
    };

}

#endif
