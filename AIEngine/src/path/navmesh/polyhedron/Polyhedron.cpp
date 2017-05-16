#include <algorithm>
#include <cmath>

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

	std::shared_ptr<CSGPolygon> Polyhedron::getOrComputeCSGPolygon() const
	{
		if(!csgPolygon)
		{
			ConvexHull2D<float> footprintConvexHull(flatPointsOnYAxis());
			std::vector<Point2<float>> cwPoints(footprintConvexHull.getPoints());
			std::reverse(cwPoints.begin(), cwPoints.end());
			csgPolygon = std::make_shared<CSGPolygon>(name, cwPoints);
		}

		return csgPolygon;
	}

	std::vector<Point2<float>> Polyhedron::flatPointsOnYAxis() const
	{
		std::vector<Point2<float>> flatPoints;
		flatPoints.reserve(points.size());

		for(const auto &polyhedronPoint : points)
		{
			flatPoints.push_back(Point2<float>(polyhedronPoint.point.X, -polyhedronPoint.point.Z));
		}

		return flatPoints;
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
		std::vector<Plane<float>> planes = buildPlanesFromFaces();
		shiftPlanes(planes, agentBound);

		for(auto &point : points)
		{
			std::vector<Plane<float>> threePlanes = findThreeNonParallelPlanes(point.faceIndices, planes);
			Point3<float> newPoint;
			if(threePlanes.size()==3)
			{
				Vector3<float> n1CrossN2 = threePlanes[0].getNormal().crossProduct(threePlanes[1].getNormal());
				Vector3<float> n2CrossN3 = threePlanes[1].getNormal().crossProduct(threePlanes[2].getNormal());
				Vector3<float> n3CrossN1 = threePlanes[2].getNormal().crossProduct(threePlanes[0].getNormal());

				newPoint = Point3<float>(n2CrossN3 * threePlanes[0].getDistanceToOrigin());
				newPoint += Point3<float>(n3CrossN1 * threePlanes[1].getDistanceToOrigin());
				newPoint += Point3<float>(n1CrossN2 * threePlanes[2].getDistanceToOrigin());
				newPoint *= -1.0 / threePlanes[0].getNormal().dotProduct(n2CrossN3);
			}else
			{ //all planes should be parallel
				Plane<float> firstPlane = planes[point.faceIndices[0]];
				float distance = computeShiftDistance(firstPlane.getNormal(), agentBound);
				newPoint = point.point.translate(firstPlane.getNormal() * distance);
			}

			point.point = newPoint;
		}

		for(auto &face : faces)
		{
			face.refreshWith(points);
		}

		csgPolygon.reset();
	}

	std::vector<Plane<float>> Polyhedron::buildPlanesFromFaces() const
	{
		std::vector<Plane<float>> planes;
		planes.reserve(faces.size());

		for(const auto &face : faces)
		{
			planes.push_back(Plane<float>(points[face.getCcwPointIndices()[0]].point, points[face.getCcwPointIndices()[1]].point, points[face.getCcwPointIndices()[2]].point));
		}

		return planes;
	}

	void Polyhedron::shiftPlanes(std::vector<Plane<float>> &planes, const BoxShape<float> &agentBound) const
	{
		for(auto &plane : planes)
		{
			float distance = computeShiftDistance(plane.getNormal(), agentBound);
			plane.setDistanceToOrigin(plane.getDistanceToOrigin() - distance);
		}
	}

	float Polyhedron::computeShiftDistance(const Vector3<float> &normal, const BoxShape<float> &agentBound) const
	{
		Vector3<float> supportPoint = Point3<float>().vector(agentBound.getSupportPoint(-normal));
		return supportPoint.dotProduct(-normal);
	}

	std::vector<Plane<float>> Polyhedron::findThreeNonParallelPlanes(const std::vector<unsigned int> &planeIndices, const std::vector<Plane<float>> &allPlanes) const
	{
		std::vector<Plane<float>> nonParallelPlanes;
		nonParallelPlanes.reserve(3);

		Plane<float> plane1 = allPlanes.at(planeIndices[0]);
		for(unsigned int i=1; i<planeIndices.size(); ++i)
		{
			Plane<float> plane2 = allPlanes.at(planeIndices[i]);
			if(plane1.getNormal().crossProduct(plane2.getNormal()).squareLength() < 0.0)
			{ //planes are parallel: continue on next plane
				continue;
			}

			for(unsigned int j=i+1; j<planeIndices.size(); ++j)
			{
				Plane<float> plane3 = allPlanes.at(planeIndices[j]);

				Vector3<float> n2CrossN3 = plane2.getNormal().crossProduct(plane3.getNormal());
				if(n2CrossN3.squareLength() < 0.0
						|| plane3.getNormal().crossProduct(plane1.getNormal()).squareLength() < 0.0
						|| plane1.getNormal().dotProduct(n2CrossN3)==0.0) //additional check due to float imprecision
				{ //planes are parallel: continue on next plane
					continue;
				}

				nonParallelPlanes = {plane1, plane2, plane3};
				return nonParallelPlanes;
			}
		}

		return nonParallelPlanes;
	}
}
