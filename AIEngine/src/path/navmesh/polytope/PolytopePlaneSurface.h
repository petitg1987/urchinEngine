#ifndef URCHINENGINE_POLYTOPEPLANESURFACE_H
#define URCHINENGINE_POLYTOPEPLANESURFACE_H

#include <vector>
#include <initializer_list>
#include "UrchinCommon.h"

#include "path/navmesh/polytope/PolytopeSurface.h"

namespace urchin
{

	class PolytopePlaneSurface : public PolytopeSurface
	{
		public:
            PolytopePlaneSurface(std::initializer_list<Point3<float>>);

			bool isWalkable(float) const override;
			Rectangle<float> computeXZRectangle() const override;

			const std::vector<Point2<float>> &getOutlineCwPoints() const override;
			Plane<float> getPlane(const Rectangle<float> &, const NavMeshAgent &) const override;
			const std::vector<CSGPolygon<float>> &getSelfObstacles() const override;
			Point3<float> computeRealPoint(const Point2<float> &, const NavMeshAgent &) const override;
			NavTopography *newNavTopography() const override;

			const std::vector<Point3<float>> &getCcwPoints() const;
			const Vector3<float> &getNormal() const;
			float getAngleToHorizontal() const;

		private:
			void buildOutlineCwPoints();

			std::vector<Point3<float>> ccwPoints;
			Vector3<float> normal;
			float angleToHorizontalInRadian;

			std::vector<Point2<float>> outlineCwPoints;
			std::vector<CSGPolygon<float>> selfObstacles;
	};

}

#endif
