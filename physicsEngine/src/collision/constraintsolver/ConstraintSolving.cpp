#include <collision/constraintsolver/ConstraintSolving.h>

namespace urchin {

    ConstraintSolving::ConstraintSolving(RigidBody& body1, RigidBody& body2, ManifoldContactPoint& manifoldContactPoint, const CommonSolvingData& commonSolvingData,
                                         const ImpulseSolvingData& impulseSolvingData) :
            body1(body1),
            body2(body2),
            manifoldContactPoint(manifoldContactPoint),
            commonData(commonSolvingData),
            impulseData(impulseSolvingData) {

    }

    RigidBody& ConstraintSolving::getBody1() const {
        return body1;
    }

    RigidBody& ConstraintSolving::getBody2() const {
        return body2;
    }

    const CommonSolvingData& ConstraintSolving::getCommonData() const {
        return commonData;
    }

    const ImpulseSolvingData& ConstraintSolving::getImpulseData() const {
        return impulseData;
    }

    AccumulatedSolvingData& ConstraintSolving::getAccumulatedData() const {
        return manifoldContactPoint.getAccumulatedSolvingData();
    }

}
