#include <UrchinCommon.h>

#include <utils/property/EagerPropertyLoader.h>

namespace urchin {

    float EagerPropertyLoader::getCollisionShapeInnerMargin() {
        static float collisionShapeInnerMargin = ConfigService::instance()->getFloatValue("collisionShape.innerMargin");
        return collisionShapeInnerMargin;
    }

    unsigned int EagerPropertyLoader::getNarrowPhaseGjkMaxIteration() {
        static unsigned int narrowPhaseGjkMaxIteration = ConfigService::instance()->getUnsignedIntValue("narrowPhase.gjkMaxIteration");
        return narrowPhaseGjkMaxIteration;
    }

    float EagerPropertyLoader::getNarrowPhaseGjkTerminationTolerance() {
        static float narrowPhaseGjkTerminationTolerance = ConfigService::instance()->getFloatValue("narrowPhase.gjkTerminationTolerance");
        return narrowPhaseGjkTerminationTolerance;
    }

    unsigned int EagerPropertyLoader::getNarrowPhaseEpaMaxIteration() {
        static unsigned int narrowPhaseEpaMaxIteration = ConfigService::instance()->getUnsignedIntValue("narrowPhase.epaMaxIteration");
        return narrowPhaseEpaMaxIteration;
    }

    float EagerPropertyLoader::getNarrowPhaseEpaTerminationTolerance() {
        static float narrowPhaseEpaTerminationTolerance = ConfigService::instance()->getFloatValue("narrowPhase.epaTerminationTolerance");
        return narrowPhaseEpaTerminationTolerance;
    }

    float EagerPropertyLoader::getNarrowPhaseContactBreakingThreshold() {
        static float narrowPhaseContactBreakingThreshold = ConfigService::instance()->getFloatValue("narrowPhase.contactBreakingThreshold");
        return narrowPhaseContactBreakingThreshold;
    }

}
