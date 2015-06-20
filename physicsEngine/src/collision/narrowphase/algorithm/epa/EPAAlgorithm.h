#ifndef ENGINE_EPAALGORITHM_H
#define ENGINE_EPAALGORITHM_H

#include <vector>
#include <limits>
#include <map>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <memory>
#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/epa/EPATriangleData.h"
#include "collision/narrowphase/algorithm/epa/EPAResult.h"
#include "collision/narrowphase/algorithm/epa/EPAResultCollide.h"
#include "collision/narrowphase/algorithm/epa/EPAResultNoCollide.h"
#include "collision/narrowphase/algorithm/epa/EPAResultInvalid.h"
#include "object/CollisionConvexObject3D.h"
#include "collision/narrowphase/algorithm/gjk/GJKResult.h"

namespace urchin
{

	template<class T> class EPAAlgorithm
	{
		public:
			EPAAlgorithm();
			~EPAAlgorithm();

			std::unique_ptr<EPAResult<T>> processEPA(const CollisionConvexObject3D &, const CollisionConvexObject3D &, const GJKResult<T> &);

		private:
			void determineInitialPoints(const Simplex<T> &, const CollisionConvexObject3D &, const CollisionConvexObject3D &,
					std::map<unsigned int, Point3<T>> &, std::map<unsigned int, Point3<T>> &, std::map<unsigned int, Point3<T>> &);
			void determineInitialTriangles(const std::map<unsigned int, Point3<T>> &, std::map<unsigned int, IndexedTriangle3D<T>> &);

			EPATriangleData<T> createTriangleData(const ConvexHull3D<T> &, unsigned int);
			typename std::map<unsigned int, EPATriangleData<T>>::const_iterator getClosestTriangleData(const typename std::map<unsigned int, EPATriangleData<T>> &) const;

			#ifdef _DEBUG
				void logMaximumIterationReach() const;
			#endif

			const unsigned int maxIteration;
			const float terminationTolerance;
			const float minDotProductTolerance;
	};

}

#endif
