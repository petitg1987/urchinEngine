#ifndef ENGINE_EPARESULTCOLLIDE_H
#define ENGINE_EPARESULTCOLLIDE_H

#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/epa/EPAResult.h"

namespace urchin
{

	template<class T> class EPAResultCollide : public EPAResult<T>
	{
		public:
			EPAResultCollide(const Point3<T> &, const Point3<T> &, const Vector3<T> &, T);
			~EPAResultCollide();

			bool isValidResult() const;

			bool isCollide() const;
			const Point3<T> &getContactPointA() const;
			const Point3<T> &getContactPointB() const;
			const Vector3<T> &getNormal() const;
			T getPenetrationDepth() const;

		private:
			Point3<T> contactPointA, contactPointB;
			Vector3<T> normal;
			T depth;
	};

}

#endif
