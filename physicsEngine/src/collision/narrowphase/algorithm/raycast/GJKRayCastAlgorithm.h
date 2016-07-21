#ifndef ENGINE_GJKRAYCASTALGORITHM_H
#define ENGINE_GJKRAYCASTALGORITHM_H

#include "collision/narrowphase/algorithm/raycast/RayCastObject.h"

namespace urchin
{

	/**
	* Implementation of GJK Ray Cast algorithm (see http://www.dtecta.com/papers/unpublished04raycast.pdf)
	*/
	class GJKRayCastAlgorithm
	{
		public:
			GJKRayCastAlgorithm();
			~GJKRayCastAlgorithm();

			void calculateTimeOfImpact(const RayCastObject &, const RayCastObject &) const;
	};

}

#endif
