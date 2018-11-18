#ifndef URCHINENGINE_GJKRESULTINVALID_H
#define URCHINENGINE_GJKRESULTINVALID_H

#include <stdexcept>
#include "UrchinCommon.h"

#include "GJKResult.h"
#include "collision/narrowphase/algorithm/utils/Simplex.h"

namespace urchin
{

	/**
	* Invalid result of GJK algorithm. Invalid result is returned by GJK algorithm when no solution is found after reach maximum iteration.
	*/
	template<class T> class GJKResultInvalid : public GJKResult<T>
	{
		public:
			bool isValidResult() const override;

			bool isCollide() const override;
			T getSeparatingDistance() const override;
			const Point3<T> &getClosestPointA() const override;
			const Point3<T> &getClosestPointB() const override;

			const Simplex<T> &getSimplex() const override;
	};

}

#endif
