#include "UrchinCommon.h"

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

	const std::shared_ptr<NavPolygon> &NavMesh::resolvePolygon(const NavTriangleRef &triangleRef) const
	{
		return polygons[triangleRef.getPolygonIndex()];
	}

	const NavTriangle &NavMesh::resolveTriangle(const NavTriangleRef &triangleRef) const
	{
		return polygons[triangleRef.getPolygonIndex()]->getTriangle(triangleRef.getTriangleIndex());
	}

	LineSegment3D<float> NavMesh::resolveEdge(const NavEdgeRef &edgeRef) const
	{
		const std::shared_ptr<NavPolygon> &navPolygon = resolvePolygon(edgeRef.getTriangleRef());
		const NavTriangle &navTriangle = navPolygon->getTriangle(edgeRef.getTriangleRef().getTriangleIndex());

		unsigned int triangleStartEdge = edgeRef.getEdgeIndex();
		unsigned int triangleEndEdge = (triangleStartEdge + 1) % 3;

		unsigned int polygonStartEdge = navTriangle.getIndex(triangleStartEdge);
		unsigned int polygonEndEdge = navTriangle.getIndex(triangleEndEdge);

		return LineSegment3D<float>(navPolygon->getPoint(polygonStartEdge), navPolygon->getPoint(polygonEndEdge));
	}
}
