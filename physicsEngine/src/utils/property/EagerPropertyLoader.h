#pragma once

namespace urchin {

    class EagerPropertyLoader {
        public:
            static float getCollisionShapeInnerMargin();

            static unsigned int getNarrowPhaseGjkMaxIteration();
            static float getNarrowPhaseGjkTerminationTolerance();
            static unsigned int getNarrowPhaseEpaMaxIteration();
            static float getNarrowPhaseEpaTerminationTolerance();
            static float getNarrowPhaseContactBreakingThreshold();
    };

}
