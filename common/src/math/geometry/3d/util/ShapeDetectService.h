#pragma once

#include <memory>
#include <vector>

#include <math/algebra/point/Point3.h>
#include <math/algebra/Quaternion.h>
#include <math/geometry/3d/shape/ConvexShape3D.h>

namespace urchin {

    class ShapeDetectService {
        public:
            struct Config {
                float voxelizationSize;
            };

            struct LocalizedShape {
                std::unique_ptr<ConvexShape3D<float>> shape;
                Point3<float> position;
                Quaternion<float> orientation;
            };

            explicit ShapeDetectService(Config config);

            std::vector<LocalizedShape> detect(const std::vector<Point3<float>>&, const std::vector<unsigned int>&) const;

        private:
            struct Mesh {
                std::vector<Point3<float>> vertices;
                std::vector<unsigned int> triangleIndices;
            };

            Mesh mergeDuplicatePoints(const std::vector<Point3<float>>&, const std::vector<unsigned int>&) const;

            std::optional<LocalizedShape> tryBuildBox(const std::vector<Point3<float>>&) const;
            std::optional<LocalizedShape> tryBuildSphere(const std::vector<Point3<float>>&) const;
            std::vector<LocalizedShape> tryBuildAABBoxes(const Mesh&) const;

            std::pair<std::size_t, std::size_t> findClosestAndFarthestPoints(const std::vector<Point3<float>>&, const Point3<float>&) const;
            std::size_t findFarthestPoint(const std::vector<Point3<float>>&, const Point3<float>&) const;

            Config config;
    };

}
