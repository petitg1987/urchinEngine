#ifndef ENGINE_GJKRESULTINVALID_H
#define ENGINE_GJKRESULTINVALID_H

#include <stdexcept>
#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/gjk/GJKResult.h"
#include "collision/narrowphase/algorithm/gjk/Simplex.h"

namespace urchin
{

	/**
	* Invalid result of GJK algorithm. Invalid result is returned by GJK algorithm when no solution is found after reach maximum iteration.
	*/
	template<class T> class GJKResultInvalid : public GJKResult<T>
	{
		public:
			GJKResultInvalid();
			~GJKResultInvalid();

			bool isValidResult() const;

			bool isCollide() const;
			T getSeparatingDistance() const;
			const Point3<T> &getClosestPointA() const;
			const Point3<T> &getClosestPointB() const;

			const Simplex<T> &getSimplex() const;
	};

}

#endif
