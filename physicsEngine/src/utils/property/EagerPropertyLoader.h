#ifndef URCHINENGINE_EAGERPROPERTYLOADER_H
#define URCHINENGINE_EAGERPROPERTYLOADER_H

#include "UrchinCommon.h"

namespace urchin {

    class EagerPropertyLoader : public Singleton<EagerPropertyLoader> {
        public:
            friend class Singleton<EagerPropertyLoader>;

            float getCollisionShapeInnerMargin() const;

            unsigned int getNarrowPhaseGjkMaxIteration() const;
            float getNarrowPhaseGjkTerminationTolerance() const;
            unsigned int getNarrowPhaseEpaMaxIteration() const;
            float getNarrowPhaseEpaTerminationTolerance() const;
            float getNarrowPhaseContactBreakingThreshold() const;

        private:
            EagerPropertyLoader();
            ~EagerPropertyLoader() override = default;

            float collisionShapeInnerMargin;

            unsigned int narrowPhaseGjkMaxIteration;
            float narrowPhaseGjkTerminationTolerance;
            unsigned int narrowPhaseEpaMaxIteration;
            float narrowPhaseEpaTerminationTolerance;
            float narrowPhaseContactBreakingThreshold;

    };

}

#endif
