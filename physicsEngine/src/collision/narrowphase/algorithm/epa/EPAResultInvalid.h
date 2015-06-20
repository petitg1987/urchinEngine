#ifndef ENGINE_EPARESULTINVALID_H
#define ENGINE_EPARESULTINVALID_H

#include <stdexcept>
#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/epa/EPAResult.h"

namespace urchin
{

	/**
	* Invalid result of EPA algorithm. Invalid result is returned by EPA algorithm when no solution is found due to numerical imprecision.
	*/
	template<class T> class EPAResultInvalid : public EPAResult<T>
	{
		public:
			EPAResultInvalid();
			~EPAResultInvalid();

			bool isValidResult() const;

			bool isCollide() const;
			const Point3<T> &getContactPointA() const;
			const Point3<T> &getContactPointB() const;
			const Vector3<T> &getNormal() const;
			T getPenetrationDepth() const;
	};

}

#endif
