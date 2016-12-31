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

			const std::vector<Point3<float>> &getPoints() const;
			const Vector3<float> &getNormal() const;

		private:
			std::vector<Point3<float>> points;
			Vector3<float> normal;

	};

}

#endif
