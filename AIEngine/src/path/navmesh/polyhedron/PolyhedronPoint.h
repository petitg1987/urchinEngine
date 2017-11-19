#ifndef URCHINENGINE_POLYHEDRONPOINT_H
#define URCHINENGINE_POLYHEDRONPOINT_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	class PolyhedronPoint
	{
		public:
			explicit PolyhedronPoint(const Point3<float> &);
			PolyhedronPoint(const Point3<float> &, const std::vector<unsigned int> &);

			void setPoint(const Point3<float> &);
			const Point3<float> &getPoint() const;

			void addFaceIndex(unsigned int);
			const std::vector<unsigned int> &getFaceIndices() const;

		private:
			Point3<float> point;
			std::vector<unsigned int> faceIndices;
	};

}

#endif
