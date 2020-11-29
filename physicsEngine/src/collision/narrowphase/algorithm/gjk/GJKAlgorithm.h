#ifndef URCHINENGINE_GJKALGORITHM_H
#define URCHINENGINE_GJKALGORITHM_H

#include <memory>
#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/gjk/result/GJKResult.h"
#include "collision/narrowphase/algorithm/gjk/result/GJKResultCollide.h"
#include "collision/narrowphase/algorithm/gjk/result/GJKResultNoCollide.h"
#include "collision/narrowphase/algorithm/gjk/result/GJKResultInvalid.h"
#include "collision/narrowphase/algorithm/utils/AlgorithmResultDeleter.h"
#include "object/CollisionConvexObject3D.h"

namespace urchin {

    /**
    * Implementation of GJK algorithm (see http://mollyrocket.com/849)
    */
    template<class T> class GJKAlgorithm {
        public:
            GJKAlgorithm();

            std::unique_ptr<GJKResult<T>, AlgorithmResultDeleter> processGJK(const CollisionConvexObject3D&, const CollisionConvexObject3D&, bool) const;

        private:
            void logMaximumIterationReach(const CollisionConvexObject3D&, const CollisionConvexObject3D&, bool) const;

            const unsigned int maxIteration;
            const float terminationTolerance;
    };

}

#endif
