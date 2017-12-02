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
			explicit PolytopePlaneSurface(const std::vector<unsigned int> &);

			void refreshWith(const std::vector<PolytopePoint> &);

			bool isWalkable(float) const override;
			std::vector<Point2<float>> getOutlineCwPoints() const override;

			const std::vector<unsigned int> &getCcwPointIndices() const;
			const std::vector<Point3<float>> &getCcwPoints() const;
			const Vector3<float> &getNormal() const;
			float getAngleToHorizontal() const;

		private:
			void checkInitialization() const;

			bool isInitialized;

			std::vector<unsigned int> ccwPointIndices;

			std::vector<Point3<float>> ccwPoints;
			Vector3<float> normal;
			float angleToHorizontalInRadian;
	};

}

#endif
