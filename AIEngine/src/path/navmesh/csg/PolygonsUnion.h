#ifndef URCHINENGINE_POLYGONSUNION_H
#define URCHINENGINE_POLYGONSUNION_H

#include <vector>
#include "UrchinCommon.h"

#include "CSGPolygon.h"

namespace urchin
{

	struct PolygonPath
	{
		PolygonPath(const ClipperLib::Path &, const std::string &);

		ClipperLib::Path path;
		std::string name;
	};

	template<class T> class PolygonsUnion : public Singleton<PolygonsUnion<T>>
	{
		public:
			friend class Singleton<PolygonsUnion<T>>;

			std::vector<CSGPolygon<T>> unionPolygons(const std::vector<CSGPolygon<T>> &) const;

		private:
			PolygonsUnion() = default;
			~PolygonsUnion() override = default;

			std::vector<PolygonPath> unionTwoPolygonPaths(const PolygonPath &, const PolygonPath &) const;

			ClipperLib::Path toPath(const CSGPolygon<T> &) const;
            CSGPolygon<T> toPolygon(const ClipperLib::Path &, const std::string &) const;

			void logInputPath(const PolygonPath &, const PolygonPath &, const std::string &, Logger::CriticalityLevel) const;
	};

}

#endif
