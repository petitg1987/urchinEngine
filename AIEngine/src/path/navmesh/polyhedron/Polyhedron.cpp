#include "Polyhedron.h"

namespace urchin
{

	/**
	 * @param faces Faces of the polyhedron. Faces must have their points in counter-clockwise to have face normal pointing outside the polyhedron.
	 * @param points All points of the polyhedrons
	 */
	Polyhedron::Polyhedron(const std::string &name, const std::vector<Face> &faces, const std::vector<Point3<float>> &points) :
			name(name),
			faces(faces),
			points(points)
	{
		footprintConvexHull = std::make_unique<ConvexHull2D<float>>(flatPointsOnYAxis(points));
	}

	const std::string Polyhedron::getName() const
	{
		return name;
	}

	const std::vector<Face> &Polyhedron::getFaces() const
	{
		return faces;
	}

	const std::vector<Point2<float>> &Polyhedron::getCwFootprintPoints() const
	{
		return footprintConvexHull->getPoints();
	}

	std::vector<Point2<float>> Polyhedron::flatPointsOnYAxis(const std::vector<Point3<float>> &points)
	{ //TODO filter points to take only those which can bother the character
		std::vector<Point2<float>> flatPoints;
		flatPoints.reserve(points.size());

		for(const auto &point : points)
		{
			flatPoints.push_back(Point2<float>(point.X, point.Z));
		}

		return flatPoints;
	}

}
