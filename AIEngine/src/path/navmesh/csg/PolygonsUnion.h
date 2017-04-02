#ifndef ENGINE_POLYGONSUNION_H
#define ENGINE_POLYGONSUNION_H

#include <vector>
#include "UrchinCommon.h"

#include "CSGPolygon.h"

namespace urchin
{

	class PolygonsUnion
	{
		public:
			std::vector<CSGPolygon> unionPolygons(const std::vector<CSGPolygon> &) const;

		private:
			std::vector<CSGPolygon> unionTwoPolygons(const CSGPolygon &, const CSGPolygon &) const;
	};

}

#endif
