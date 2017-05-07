#include <algorithm>

#include "Polyhedron.h"

namespace urchin
{

	/**
	 * @param faces Faces of the polyhedron. Faces must have their points in counter-clockwise to have face normal pointing outside the polyhedron.
	 * @param points All points of the polyhedrons
	 */
	Polyhedron::Polyhedron(const std::string &name, const std::vector<Face> &faces, const std::vector<PolyhedronPoint> &points) :
			name(name),
			faces(faces),
			points(points),
			walkableCandidate(true)
	{

	}

	const std::string Polyhedron::getName() const
	{
		return name;
	}

	const std::vector<Face> &Polyhedron::getFaces() const
	{
		return faces;
	}

	const Face &Polyhedron::getFace(unsigned int faceIndex) const
	{
		return faces[faceIndex];
	}

	void Polyhedron::setWalkableCandidate(bool walkableCandidate)
	{
		this->walkableCandidate = walkableCandidate;
	}

	bool Polyhedron::isWalkableCandidate() const
	{
		return walkableCandidate;
	}

	std::vector<Point2<float>> Polyhedron::computeCwFootprintPoints(const BoxShape<float> &agentBound) const
	{
		//TODO :
		//1) expand 'this' polyhedron (not to be done in this method ?)
		//2) take only points below the walkable surface
		//3) create 2d convex hull with these points
		ConvexHull2D<float> footprintConvexHull(flatPointsOnYAxis(points));
		std::unique_ptr<ConvexHull2D<float>> expandedFootprintConvexHull = footprintConvexHull.resize(agentBound.getHalfSize(0));

		std::vector<Point2<float>> points(expandedFootprintConvexHull->getPoints());
		std::reverse(points.begin(), points.end());

		return points;
	}

	std::vector<Point2<float>> Polyhedron::flatPointsOnYAxis(const std::vector<PolyhedronPoint> &polyhedronPoints) const
	{
		std::vector<Point2<float>> flatPoints;
		flatPoints.reserve(polyhedronPoints.size());

		for(const auto &polyhedronPoint : polyhedronPoints)
		{
			flatPoints.push_back(Point2<float>(polyhedronPoint.point.X, -polyhedronPoint.point.Z));
		}

		return flatPoints;
	}

}
