#ifndef ENGINE_RESIZECONVEXHULLSERVICE_H
#define ENGINE_RESIZECONVEXHULLSERVICE_H

#include <memory>
#include "UrchinCommon.h"

namespace urchin
{

	class ResizeConvexHullService : public Singleton<ResizeConvexHullService>
	{
		public:
			friend class Singleton<ResizeConvexHullService>;

			std::unique_ptr<ConvexHull3D<float>> resizeConvexHull(const ConvexHull3D<float> &, float) const;

		private:
			ResizeConvexHullService();
			virtual ~ResizeConvexHullService();

			void buildPlanesFromConvexHull(const ConvexHull3D<float> &, std::vector<Plane<float>> &) const;
			bool isPlaneAlreadyExist(const std::vector<Plane<float>> &, const Plane<float> &) const;
			void shiftPlanes(const std::vector<Plane<float>> &, std::vector<Plane<float>> &, float) const;
			bool isPointInsidePlanes(const std::vector<Plane<float>> &, const Point3<float> &) const;
	};

}

#endif
