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
            ConstraintSolving(RigidBody*, RigidBody*, ManifoldContactPoint&);

            RigidBody* getBody1();
            RigidBody* getBody2();

            const CommonSolvingData& getCommonData();
            void setCommonData(const CommonSolvingData&);

            const ImpulseSolvingData& getImpulseData();
            void setImpulseData(const ImpulseSolvingData&);

            AccumulatedSolvingData& getAccumulatedData();

        private:
            RigidBody* body1;
            RigidBody* body2;
            ManifoldContactPoint& manifoldContactPoint;

            CommonSolvingData commonData;
            ImpulseSolvingData impulseData;
    };

}
