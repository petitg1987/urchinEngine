#pragma once

#include <math/geometry/3d/MeshData.h>

namespace urchin {

    class MeshSimplificationService {
        public:
            struct Config {
                float edgeDistanceThreshold;
            };

            struct Edge {
                struct Hash {
                    uint64_t operator()(const Edge&) const;
                };

                bool operator==(const Edge&) const = default;

                uint32_t vertexIndex1;
                uint32_t vertexIndex2;
            };

            explicit MeshSimplificationService(Config);

            MeshData simplify(const MeshData&) const;

        private:
            MeshData mergeDuplicateVertices(const MeshData&) const;

            MeshData collapseShortEdge(const MeshData&) const;
            void cleanUnusedVertices(std::vector<Point3<float>>&, std::vector<std::array<uint32_t, 3>>&) const;

            Config config;
    };

}
