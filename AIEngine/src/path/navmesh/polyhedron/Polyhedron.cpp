#include <algorithm>

#include "Polyhedron.h"

namespace urchin
{

	/**
	 * @param faces Indexed faces of the polyhedron. Faces must have their points in counter-clockwise to have face normal pointing outside the polyhedron.
	 * @param points All points of the polyhedrons
	 */
	Polyhedron::Polyhedron(const std::string &name, std::vector<PolyhedronFace> &faces, const std::vector<PolyhedronPoint> &points) :
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

	const std::vector<PolyhedronFace> &Polyhedron::getFaces() const
	{
		return faces;
	}

	const PolyhedronFace &Polyhedron::getFace(unsigned int faceIndex) const
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

	void Polyhedron::expand(const BoxShape<float> &agentBound)
	{
		//TODO expand faces & points + bevel planes in TODO.txt

		for(auto &face : faces)
		{
			face.refreshWith(points);
		}
	}

	std::vector<Point2<float>> Polyhedron::computeCwFootprintPoints(const PolyhedronFace &walkableFace) const
	{
		//TODO Take only points below the 'walkableFace'
		//TODO Create 2d convex hull with these points
		ConvexHull2D<float> footprintConvexHull(flatPointsOnYAxis(points));
		std::vector<Point2<float>> points(footprintConvexHull.getPoints());
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
