#include "NavMesh.h"

namespace urchin
{

	//static
	unsigned int NavMesh::nextId = 0;

	NavMesh::NavMesh()	:
		id(nextId++)
	{

	}

	unsigned int NavMesh::getId() const
	{
		return id;
	}

	unsigned int NavMesh::addPolygon(std::shared_ptr<NavPolygon> polygon)
	{
		polygons.push_back(polygon);
		return polygons.size()-1;
	}

	const std::vector<std::shared_ptr<NavPolygon>> &NavMesh::getPolygons() const
	{
		return polygons;
	}
}
