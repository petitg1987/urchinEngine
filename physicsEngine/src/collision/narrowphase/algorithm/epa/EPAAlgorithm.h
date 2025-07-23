#pragma once

#include <map>
#include <cmath>
#include <UrchinCommon.h>

#include "object/CollisionConvexObject3D.h"
#include "collision/narrowphase/algorithm/epa/EPATriangleData.h"
#include "collision/narrowphase/algorithm/epa/EPAResult.h"

namespace urchin {

    template<class T> class EPAAlgorithm {
        public:
           EPAResult<T> processEPA(const CollisionConvexObject3D&, const CollisionConvexObject3D&, const GJKResult<T>&) const;

        private:
            EPAResult<T> handleSubTriangle(const CollisionConvexObject3D&, const CollisionConvexObject3D&) const;

            void determineInitialPoints(const Simplex<T>&, const CollisionConvexObject3D&, const CollisionConvexObject3D&,
                    std::map<std::size_t, ConvexHullPoint<T>>&, std::map<std::size_t, Point3<T>>&, std::map<std::size_t, Point3<T>>&) const;
            void determineInitialTriangles(std::map<std::size_t, ConvexHullPoint<T>>&, std::map<std::size_t, IndexedTriangle3D<T>>&) const;

            typename std::map<std::size_t, EPATriangleData<T>>::const_iterator getClosestTriangleData(const std::map<std::size_t, EPATriangleData<T>>&) const;
            EPATriangleData<T> createTriangleData(const ConvexHullShape3D<T>&, std::size_t) const;

            void logInputData(std::string_view, const CollisionConvexObject3D&, const CollisionConvexObject3D&, const GJKResult<T>&) const;

            static constexpr unsigned int MAX_ITERATION = 30;
            static constexpr float TERMINATION_TOLERANCE = 0.01f;
    };

}
