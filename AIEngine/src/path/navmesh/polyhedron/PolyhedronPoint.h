#ifndef URCHINENGINE_POLYHEDRONPOINT_H
#define URCHINENGINE_POLYHEDRONPOINT_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	class PolyhedronPoint
	{
		public:
			Point3<float> point;
			std::vector<unsigned int> faceIndices;
	};

}

#endif
