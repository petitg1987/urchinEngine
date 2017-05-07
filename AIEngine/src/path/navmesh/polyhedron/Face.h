#ifndef ENGINE_FACE_H
#define ENGINE_FACE_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	class PolyhedronPoint;

	class Face
	{
		public:
			Face(const std::vector<Point3<float>> &); //TODO use indices

			const std::vector<Point3<float>> &getCcwPoints() const;
			const std::vector<Point3<float>> getCwPoints() const;

			const Vector3<float> &getNormal() const;

			float getAngleToHorizontal() const;

			void setWalkableCandidate(bool);
			bool isWalkableCandidate() const;

		private:
			std::vector<Point3<float>> ccwPoints;

			Vector3<float> normal;
			float angleToHorizontalInRadian;

			bool walkableCandidate;
	};

}

#endif
