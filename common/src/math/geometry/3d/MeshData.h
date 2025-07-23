#pragma once

#include <cstdint>
#include <vector>
#include <array>

#include "math/algebra/point/Point3.h"

namespace urchin {

    class MeshData {
        public:
            MeshData(std::vector<Point3<float>>, std::vector<std::array<uint32_t, 3>>);

            void addNewMesh(const std::vector<Point3<float>>&, const std::vector<std::array<uint32_t, 3>>&);

            const std::vector<Point3<float>>& getVertices() const;
            const std::vector<std::array<uint32_t, 3>>& getTrianglesIndices() const;

        private:
            std::vector<Point3<float>> vertices;
            std::vector<std::array<uint32_t, 3>> trianglesIndices;
    };

}
