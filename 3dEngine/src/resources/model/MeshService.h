#ifndef URCHINENGINE_MESHSERVICE_H
#define URCHINENGINE_MESHSERVICE_H

#include "UrchinCommon.h"

namespace urchin {
    class ConstMesh;
    struct Bone;

    class MeshService : public Singleton<MeshService> {
        public:
            friend class Singleton<MeshService>;

            void computeVertices(const ConstMesh*, const std::vector<Bone>&, std::vector<Point3<float>>&);
            void computeNormalsAndTangents(const ConstMesh*, const std::vector<Point3<float>>&, std::vector<Vector3<float>>&, std::vector<Vector3<float>>&);

        private:
            MeshService();
            ~MeshService() override = default;

            Vector3<float> computeWeightedVertexNormal(unsigned int, unsigned int, unsigned int, const std::vector<Point3<float>>&);
    };

}

#endif
