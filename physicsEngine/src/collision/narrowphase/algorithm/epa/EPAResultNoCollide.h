#ifndef URCHINENGINE_EPARESULTNOCOLLIDE_H
#define URCHINENGINE_EPARESULTNOCOLLIDE_H

#include <stdexcept>
#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/epa/EPAResult.h"

namespace urchin
{

	template<class T> class EPAResultNoCollide : public EPAResult<T>
	{
		public:
			EPAResultNoCollide();
			~EPAResultNoCollide();

			bool isValidResult() const;

			bool isCollide() const;
			const Point3<T> &getContactPointA() const;
			const Point3<T> &getContactPointB() const;
			const Vector3<T> &getNormal() const;
			T getPenetrationDepth() const;
	};

}

#endif
