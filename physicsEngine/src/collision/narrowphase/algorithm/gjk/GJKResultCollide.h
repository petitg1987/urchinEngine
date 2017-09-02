#ifndef URCHINENGINE_GJKRESULTCOLLIDE_H
#define URCHINENGINE_GJKRESULTCOLLIDE_H

#include <stdexcept>
#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/gjk/GJKResult.h"
#include "collision/narrowphase/algorithm/utils/Simplex.h"

namespace urchin
{

	template<class T> class GJKResultCollide : public GJKResult<T>
	{
		public:
			GJKResultCollide(const Simplex<T> &);
			~GJKResultCollide();

			bool isValidResult() const;

			bool isCollide() const;
			T getSeparatingDistance() const;
			const Point3<T> &getClosestPointA() const;
			const Point3<T> &getClosestPointB() const;

			const Simplex<T> &getSimplex() const;

		private:
			Simplex<T> simplex;
	};

}

#endif
