#ifndef ENGINE_POLYHEDRONFACE_H
#define ENGINE_POLYHEDRONFACE_H

#include <vector>
#include "UrchinCommon.h"

#include "path/navmesh/polyhedron/PolyhedronPoint.h"
#include "path/navmesh/csg/CSGPolygon.h"

namespace urchin
{

	class PolyhedronFace
	{
		public:
			PolyhedronFace(const std::vector<unsigned int> &);

			void refreshWith(const std::vector<PolyhedronPoint> &);

			const std::vector<unsigned int> &getCcwPointIndices() const;
			const std::vector<Point3<float>> &getCcwPoints() const;
			const Vector3<float> &getNormal() const;
			float getAngleToHorizontal() const;

			void setWalkableCandidate(bool);
			bool isWalkableCandidate() const;

			CSGPolygon computeCSGPolygon(const std::string &) const;

		private:
			void checkInitialization() const;

			std::vector<Point2<float>> flatPointsOnYAxis() const;

			bool isInitialized;

			std::vector<unsigned int> ccwPointIndices;

			std::vector<Point3<float>> ccwPoints;
			Vector3<float> normal;
			float angleToHorizontalInRadian;

			bool walkableCandidate;
	};

}

#endif
