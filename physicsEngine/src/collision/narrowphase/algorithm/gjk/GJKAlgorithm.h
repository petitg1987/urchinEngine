#ifndef ENGINE_GJKALGORITHM_H
#define ENGINE_GJKALGORITHM_H

#include <memory>
#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/gjk/GJKResult.h"
#include "collision/narrowphase/algorithm/gjk/GJKResultCollide.h"
#include "collision/narrowphase/algorithm/gjk/GJKResultNoCollide.h"
#include "collision/narrowphase/algorithm/gjk/GJKResultInvalid.h"
#include "object/CollisionConvexObject3D.h"

namespace urchin
{

	/**
	* Implementation of GJK algorithm (see http://mollyrocket.com/849)
	*/
	template<class T> class GJKAlgorithm
	{
		public:
			GJKAlgorithm();

			std::unique_ptr<GJKResult<T>> processGJK(const CollisionConvexObject3D &, const CollisionConvexObject3D &, bool) const;

		private:
			void logMaximumIterationReach(const CollisionConvexObject3D &, const CollisionConvexObject3D &, bool) const;

			const unsigned int maxIteration;
			const float terminationTolerance;
	};

}

#endif
