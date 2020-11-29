#ifndef URCHINENGINE_CONSTRAINTSOLVING_H
#define URCHINENGINE_CONSTRAINTSOLVING_H

#include "UrchinCommon.h"

#include "collision/ManifoldContactPoint.h"
#include "collision/constraintsolver/solvingdata/CommonSolvingData.h"
#include "collision/constraintsolver/solvingdata/ImpulseSolvingData.h"
#include "collision/constraintsolver/solvingdata/AccumulatedSolvingData.h"

namespace urchin {

    /**
    * Constraint data container
    */
    class ConstraintSolving {
        public:
            ConstraintSolving(WorkRigidBody*, WorkRigidBody*, ManifoldContactPoint&);

            WorkRigidBody* getBody1();
            WorkRigidBody* getBody2();

            const CommonSolvingData& getCommonData();
            void setCommonData(const CommonSolvingData&);

            const ImpulseSolvingData& getImpulseData();
            void setImpulseData(const ImpulseSolvingData&);

            AccumulatedSolvingData& getAccumulatedData();

        private:
            WorkRigidBody *body1;
            WorkRigidBody *body2;
            ManifoldContactPoint &manifoldContactPoint;

            CommonSolvingData commonData;
            ImpulseSolvingData impulseData;
    };

}

#endif
