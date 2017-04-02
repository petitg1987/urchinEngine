#include <algorithm>

#include "PolygonsUnion.h"

namespace urchin
{

	std::vector<CSGPolygon> PolygonsUnion::unionPolygons(const std::vector<CSGPolygon> &polygons) const
	{
		std::vector<CSGPolygon> mergedPolygons;
		mergedPolygons.reserve(polygons.size()/2 + 1); //estimated memory size

		std::vector<CSGPolygon> allPolygons = polygons; //TODO check and improve performance on copy constructor
		allPolygons.reserve(allPolygons.size() + mergedPolygons.size());

		while(allPolygons.size() > 0)
		{
			bool isPolygonsMerged = false;
			for(unsigned int i=1; i<allPolygons.size(); ++i)
			{
				std::vector<CSGPolygon> result = unionTwoPolygons(allPolygons[0], allPolygons[i]);
				if(result.size()==1)
				{
					isPolygonsMerged = true;

					VectorEraser::erase(allPolygons, i); //TODO check and improve performance on copy constructor
					VectorEraser::erase(allPolygons, 0);

					allPolygons.push_back(result[0]);

					break;
				}
			}
			if(!isPolygonsMerged)
			{
				mergedPolygons.push_back(allPolygons[0]);
				VectorEraser::erase(allPolygons, 0);
			}
		}

		return mergedPolygons;
	}

	std::vector<CSGPolygon> PolygonsUnion::unionTwoPolygons(const CSGPolygon &polygon1, const CSGPolygon &polygon2) const
	{
		std::vector<CSGPolygon> mergedPolygons;
		mergedPolygons.reserve(2);

		//TODO merge polygon

		return mergedPolygons;
	}

}
