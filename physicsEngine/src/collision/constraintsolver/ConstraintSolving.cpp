#include "collision/constraintsolver/ConstraintSolving.h"

namespace urchin {

    ConstraintSolving::ConstraintSolving(RigidBody* body1, RigidBody* body2, ManifoldContactPoint& manifoldContactPoint) :
        body1(body1),
        body2(body2),
        manifoldContactPoint(manifoldContactPoint) {

    }

    RigidBody* ConstraintSolving::getBody1() {
        return body1;
    }

    RigidBody* ConstraintSolving::getBody2() {
        return body2;
    }

    const CommonSolvingData& ConstraintSolving::getCommonData() {
        return commonData;
    }

    void ConstraintSolving::setCommonData(const CommonSolvingData& commonSolvingData) {
        this->commonData = commonSolvingData;
    }

    const ImpulseSolvingData& ConstraintSolving::getImpulseData() {
        return impulseData;
    }

    void ConstraintSolving::setImpulseData(const ImpulseSolvingData& impulseSolvingData) {
        this->impulseData = impulseSolvingData;
    }

    AccumulatedSolvingData& ConstraintSolving::getAccumulatedData() {
        return manifoldContactPoint.getAccumulatedSolvingData();
    }

}
