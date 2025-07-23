#include "collision/constraintsolver/ConstraintSolvingData.h"

namespace urchin {

    ConstraintSolvingData::ConstraintSolvingData(RigidBody& body1, RigidBody& body2, ManifoldContactPoint& manifoldContactPoint, const CommonSolvingData& commonSolvingData,
                                         const ImpulseSolvingData& impulseSolvingData) :
            body1(body1),
            body2(body2),
            manifoldContactPoint(manifoldContactPoint),
            commonData(commonSolvingData),
            impulseData(impulseSolvingData) {

    }

    RigidBody& ConstraintSolvingData::getBody1() const {
        return body1;
    }

    RigidBody& ConstraintSolvingData::getBody2() const {
        return body2;
    }

    const CommonSolvingData& ConstraintSolvingData::getCommonData() const {
        return commonData;
    }

    const ImpulseSolvingData& ConstraintSolvingData::getImpulseData() const {
        return impulseData;
    }

    AccumulatedSolvingData& ConstraintSolvingData::getAccumulatedData() const {
        return manifoldContactPoint.getAccumulatedSolvingData();
    }

}
