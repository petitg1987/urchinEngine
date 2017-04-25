#ifndef ENGINE_FACE_H
#define ENGINE_FACE_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	class Face
	{
		public:
			Face(const std::vector<Point3<float>> &);

			const std::vector<Point3<float>> &getCcwPoints() const;
			const std::vector<Point3<float>> getCwPoints() const;

			const Vector3<float> &getNormal() const;

			void setWalkableCandidate(bool);
			bool isWalkableCandidate() const;

			float getAngleToHorizontal() const;

		private:
			std::vector<Point3<float>> ccwPoints;
			Vector3<float> normal;

			bool walkableCandidate;

			float angleToHorizontalInRadian;
	};

}

#endif
