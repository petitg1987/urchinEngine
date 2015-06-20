#ifndef ENGINE_INERTIACALCULATION_H
#define ENGINE_INERTIACALCULATION_H

#include "UrchinCommon.h"

#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	class InertiaCalculation
	{
		public:
			static Matrix3<float> computeInverseWorldInertia(const Vector3<float> &, const PhysicsTransform &);

		private:
			InertiaCalculation();
			~InertiaCalculation();
	};

}

#endif
