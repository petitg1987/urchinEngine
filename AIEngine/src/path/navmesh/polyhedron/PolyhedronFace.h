#ifndef ENGINE_POLYHEDRONFACE_H
#define ENGINE_POLYHEDRONFACE_H

#include <vector>
#include "UrchinCommon.h"

#include "path/navmesh/polyhedron/PolyhedronPoint.h"

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

		private:
			std::vector<unsigned int> ccwPointIndices;

			std::vector<Point3<float>> ccwPoints;
			Vector3<float> normal;
			float angleToHorizontalInRadian;

			bool walkableCandidate;
	};

}

#endif
