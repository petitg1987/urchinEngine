#pragma once

#include <UrchinCommon.h>

#include <body/model/RigidBody.h>
#include <collision/ManifoldContactPoint.h>
#include <collision/constraintsolver/solvingdata/CommonSolvingData.h>
#include <collision/constraintsolver/solvingdata/ImpulseSolvingData.h>
#include <collision/constraintsolver/solvingdata/AccumulatedSolvingData.h>

namespace urchin {

    /**
    * Constraint data container
    */
    class ConstraintSolving {
        public:
            ConstraintSolving(RigidBody&, RigidBody&, ManifoldContactPoint&, const CommonSolvingData&, const ImpulseSolvingData&);

            RigidBody& getBody1() const;
            RigidBody& getBody2() const;

            const CommonSolvingData& getCommonData() const;
            const ImpulseSolvingData& getImpulseData() const;
            AccumulatedSolvingData& getAccumulatedData() const;

        private:
            RigidBody& body1;
            RigidBody& body2;
            ManifoldContactPoint& manifoldContactPoint;

            CommonSolvingData commonData;
            ImpulseSolvingData impulseData;
    };

}
