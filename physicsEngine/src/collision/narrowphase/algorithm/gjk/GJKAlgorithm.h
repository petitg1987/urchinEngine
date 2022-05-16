#pragma once

#include <memory>
#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/gjk/GJKResult.h>
#include <object/CollisionConvexObject3D.h>

namespace urchin {

    /**
    * Implementation of GJK algorithm (see http://mollyrocket.com/849)
    */
    template<class T> class GJKAlgorithm {
        public:
            GJKResult<T> processGJK(const CollisionConvexObject3D&, const CollisionConvexObject3D&, bool) const;

        private:
            void logMaximumIterationReach(const CollisionConvexObject3D&, const CollisionConvexObject3D&, bool) const;

            static constexpr unsigned int MAX_ITERATION = 20;
            static constexpr float TERMINATION_TOLERANCE = 0.00015f;
    };

}
