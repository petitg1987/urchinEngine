#pragma once

#include <memory>
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/gjk/GJKAlgorithm.h>
#include <collision/narrowphase/algorithm/gjk/GJKResult.h>
#include <object/CollisionConvexObject3D.h>

namespace urchin {

    /**
    * Implementation of GJK algorithm (see http://mollyrocket.com/849)
    */
    template<class T> class GJKAlgorithm {
        public:
            template<class CONVEX_OBJ> GJKResult<T> processGJK(const CONVEX_OBJ&, const CONVEX_OBJ&) const;

        private:
            template<class CONVEX_OBJ> void logMaximumIterationReach(const CONVEX_OBJ&, const CONVEX_OBJ&) const;

            static constexpr unsigned int MAX_ITERATION = 20;
            static constexpr float TERMINATION_TOLERANCE = 0.00015f;
    };

    #include "GJKAlgorithm.inl"

}
