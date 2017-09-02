#ifndef URCHINENGINE_GJKRESULTNOCOLLIDE_H
#define URCHINENGINE_GJKRESULTNOCOLLIDE_H

#include <stdexcept>
#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/gjk/GJKResult.h"
#include "collision/narrowphase/algorithm/utils/Simplex.h"

namespace urchin
{

	template<class T> class GJKResultNoCollide : public GJKResult<T>
	{
		public:
			GJKResultNoCollide(T, const Simplex<T> &);
			~GJKResultNoCollide();

			bool isValidResult() const;

			bool isCollide() const;
			T getSeparatingDistance() const;
			const Point3<T> &getClosestPointA() const;
			const Point3<T> &getClosestPointB() const;

			const Simplex<T> &getSimplex() const;

		private:
			T separatingDistance;
			Point3<T> closestPointA, closestPointB;

			Simplex<T> simplex;
	};

}

#endif
