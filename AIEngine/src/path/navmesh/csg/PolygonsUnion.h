#ifndef URCHINENGINE_POLYGONSUNION_H
#define URCHINENGINE_POLYGONSUNION_H

#include <vector>
#include "UrchinCommon.h"

#include "CSGPolygon.h"
#include "path/navmesh/csg/CSGPolygonPath.h"

namespace urchin
{

	template<class T> class PolygonsUnion : public Singleton<PolygonsUnion<T>>
	{
		public:
			friend class Singleton<PolygonsUnion<T>>;

			std::vector<CSGPolygon<T>> unionPolygons(const std::vector<CSGPolygon<T>> &) const;

		private:
			PolygonsUnion() = default;
			~PolygonsUnion() override = default;

			std::vector<CSGPolygonPath> unionTwoPolygonPaths(const CSGPolygonPath &, const CSGPolygonPath &) const;

			void logInputData(const std::vector<CSGPolygon<T>> &, const std::string &, Logger::CriticalityLevel) const;
	};

}

#endif
