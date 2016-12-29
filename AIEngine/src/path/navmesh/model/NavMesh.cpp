#include "NavMesh.h"

namespace urchin
{

	NavMesh::NavMesh()
	{

	}

	unsigned int NavMesh::addPolygon(const NavPolygon &polygon)
	{
		polygons.push_back(polygon);
		return polygons.size()-1;
	}

	const std::vector<NavPolygon> &NavMesh::getPolygons() const
	{
		return polygons;
	}

	const NavPolygon &NavMesh::getPolygon(unsigned int index) const
	{
		return polygons[index];
	}
}
