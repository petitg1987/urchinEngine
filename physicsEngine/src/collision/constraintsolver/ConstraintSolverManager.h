#ifndef URCHINENGINE_CONSTRAINTSOLVERMANAGER_H
#define URCHINENGINE_CONSTRAINTSOLVERMANAGER_H

#include <vector>
#include <map>
#include "UrchinCommon.h"

#include "collision/constraintsolver/ConstraintSolving.h"
#include "collision/constraintsolver/solvingdata/CommonSolvingData.h"
#include "collision/constraintsolver/solvingdata/ImpulseSolvingData.h"
#include "body/BodyManager.h"
#include "collision/ManifoldResult.h"
#include "utils/pool/FixedSizePool.h"
#include "body/work/WorkRigidBody.h"

namespace urchin
{

	class ConstraintSolverManager
	{
		public:
			ConstraintSolverManager();
			~ConstraintSolverManager();

			void solveConstraints(float, std::vector<ManifoldResult> &);

		private:
			void setupConstraints(std::vector<ManifoldResult> &, float);
			void solveConstraints();

			CommonSolvingData fillCommonSolvingData(const ManifoldResult &, const ManifoldContactPoint &);
			ImpulseSolvingData fillImpulseSolvingData(const CommonSolvingData &, float) const;

			void solveNormalConstraint(ConstraintSolving *);
			void solveTangentConstraint(ConstraintSolving *);

			void applyImpulse(WorkRigidBody *, WorkRigidBody *, const CommonSolvingData &, const Vector3<float> &);
			Vector3<float> computeRelativeVelocity(const CommonSolvingData &) const;
			Vector3<float> computeTangent(const CommonSolvingData &, const Vector3<float> &) const;

			std::vector<ConstraintSolving *> constraintsSolving;
			FixedSizePool<ConstraintSolving> *constraintSolvingPool;

			const unsigned int constraintSolverIteration;
			const float biasFactor;
			const bool useWarmStarting;
			const float restitutionVelocityThreshold;
	};

}

#endif
