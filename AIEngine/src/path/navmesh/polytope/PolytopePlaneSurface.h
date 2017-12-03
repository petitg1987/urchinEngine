#ifndef URCHINENGINE_POLYTOPEPLANESURFACE_H
#define URCHINENGINE_POLYTOPEPLANESURFACE_H

#include <vector>
#include "UrchinCommon.h"

#include "path/navmesh/polytope/PolytopeSurface.h"
#include "path/navmesh/polytope/PolytopePoint.h"

namespace urchin
{

	class PolytopePlaneSurface : public PolytopeSurface
	{
		public:
			PolytopePlaneSurface(const std::vector<unsigned int> &, const std::vector<PolytopePoint> &);

			bool isWalkable(float) const override;
			Rectangle<float> computeXZRectangle() const override;

			std::vector<Point2<float>> getOutlineCwPoints() const override;
			Plane<float> getPlaneIn(const Rectangle<float> &) const override;
			Point3<float> elevatePoint(const Point2<float> &, const NavMeshAgent &) const override;

			const std::vector<unsigned int> &getCcwPointIndices() const;
			const std::vector<Point3<float>> &getCcwPoints() const;
			const Vector3<float> &getNormal() const;
			float getAngleToHorizontal() const;

		private:
			std::vector<unsigned int> ccwPointIndices;

			std::vector<Point3<float>> ccwPoints;
			Vector3<float> normal;
			float angleToHorizontalInRadian;
	};

}

#endif
