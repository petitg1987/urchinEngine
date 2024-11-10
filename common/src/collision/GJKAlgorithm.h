#pragma once

#include <cmath>
#include <sstream>

#include <collision/GJKResult.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/vector/Vector3.h>
#include <profiler/ScopeProfiler.h>
#include <math/geometry/3d/Simplex.h>
#include <logger/Logger.h>

namespace urchin {

    /**
    * Implementation of GJK algorithm (see http://mollyrocket.com/849)
    */
    template<class T> class GJKAlgorithm {
        public:
            template<class CONVEX_OBJ1, class CONVEX_OBJ2> GJKResult<T> processGJK(const CONVEX_OBJ1&, const CONVEX_OBJ2&) const;

        private:
            template<class CONVEX_OBJ1, class CONVEX_OBJ2> void logMaximumIterationReach(const CONVEX_OBJ1&, const CONVEX_OBJ2&) const;

            static constexpr unsigned int MAX_ITERATION = 20;
            static constexpr float TERMINATION_TOLERANCE = 0.00015f;
    };

    #include "GJKAlgorithm.inl"

}
