#ifndef ENGINE_EPAALGORITHM_H
#define ENGINE_EPAALGORITHM_H

#include <vector>
#include <limits>
#include <map>
#include <cmath>
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

			std::unique_ptr<EPAResult<T>> processEPA(const CollisionConvexObject3D &, const CollisionConvexObject3D &, const GJKResult<T> &) const;

		private:
			void determineInitialPoints(const Simplex<T> &, const CollisionConvexObject3D &, const CollisionConvexObject3D &,
					std::map<unsigned int, Point3<T>> &, std::map<unsigned int, Point3<T>> &, std::map<unsigned int, Point3<T>> &) const;
			void determineInitialTriangles(const std::map<unsigned int, Point3<T>> &, std::map<unsigned int, IndexedTriangle3D<T>> &, std::map<unsigned int, unsigned int> &) const;

			EPATriangleData<T> createTriangleData(const ConvexHullShape3D<T> &, unsigned int) const;
			typename std::map<unsigned int, EPATriangleData<T>>::const_iterator getClosestTriangleData(const typename std::map<unsigned int, EPATriangleData<T>> &) const;

			const unsigned int maxIteration;
			const float terminationTolerance;
	};

}

#endif
