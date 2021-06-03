#include <utils/property/EagerPropertyLoader.h>

namespace urchin {
    EagerPropertyLoader::EagerPropertyLoader() :
        Singleton<EagerPropertyLoader>() {
        collisionShapeInnerMargin = ConfigService::instance()->getFloatValue("collisionShape.innerMargin");

        narrowPhaseGjkMaxIteration = ConfigService::instance()->getUnsignedIntValue("narrowPhase.gjkMaxIteration");
        narrowPhaseGjkTerminationTolerance = ConfigService::instance()->getFloatValue("narrowPhase.gjkTerminationTolerance");
        narrowPhaseEpaMaxIteration = ConfigService::instance()->getUnsignedIntValue("narrowPhase.epaMaxIteration");
        narrowPhaseEpaTerminationTolerance = ConfigService::instance()->getFloatValue("narrowPhase.epaTerminationTolerance");
        narrowPhaseContactBreakingThreshold = ConfigService::instance()->getFloatValue("narrowPhase.contactBreakingThreshold");
    }

    float EagerPropertyLoader::getCollisionShapeInnerMargin() const {
        return collisionShapeInnerMargin;
    }

    unsigned int EagerPropertyLoader::getNarrowPhaseGjkMaxIteration() const {
        return narrowPhaseGjkMaxIteration;
    }

    float EagerPropertyLoader::getNarrowPhaseGjkTerminationTolerance() const {
        return narrowPhaseGjkTerminationTolerance;
    }

    unsigned int EagerPropertyLoader::getNarrowPhaseEpaMaxIteration() const {
        return narrowPhaseEpaMaxIteration;
    }

    float EagerPropertyLoader::getNarrowPhaseEpaTerminationTolerance() const {
        return narrowPhaseEpaTerminationTolerance;
    }

    float EagerPropertyLoader::getNarrowPhaseContactBreakingThreshold() const {
        return narrowPhaseContactBreakingThreshold;
    }

}
