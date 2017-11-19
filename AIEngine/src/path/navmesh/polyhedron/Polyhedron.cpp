#include <algorithm>
#include <cmath>

#include "Polyhedron.h"

namespace urchin
{

	Polyhedron::Polyhedron() :
			walkableCandidate(true),
			obstacleCandidate(true)
	{

	}

	/**
	 * @param faces Indexed faces of the polyhedron. Faces must have their points in counter-clockwise to have face normal pointing outside the polyhedron.
	 * @param points All points of the polyhedrons
	 */
	Polyhedron::Polyhedron(const std::string &name, std::vector<PolyhedronFace> &faces, const std::vector<PolyhedronPoint> &points) :
			name(name),
			faces(faces),
			points(points),
			walkableCandidate(true),
            obstacleCandidate(true)
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

	const std::vector<PolyhedronPoint> &Polyhedron::getPoints() const
	{
		return points;
	}

	void Polyhedron::setWalkableCandidate(bool walkableCandidate)
	{
		this->walkableCandidate = walkableCandidate;
	}

	bool Polyhedron::isWalkableCandidate() const
	{
		return walkableCandidate;
	}

    void Polyhedron::setObstacleCandidate(bool obstacleCandidate)
    {
        Polyhedron::obstacleCandidate = obstacleCandidate;
    }

    bool Polyhedron::isObstacleCandidate() const
    {
        return obstacleCandidate;
    }

	void Polyhedron::expand(const NavMeshAgent &agent)
	{
		std::vector<Plane<float>> planes = buildPlanesFromFaces();
		shiftPlanes(planes, agent);

		for(auto &polyhedronPoint : points)
		{
            if(polyhedronPoint.getFaceIndices().size()==1)
            { //face is individual
				Plane<float> plane = planes[polyhedronPoint.getFaceIndices()[0]];
                float distance = agent.computeExpandDistance(plane.getNormal());
				polyhedronPoint.setPoint(polyhedronPoint.getPoint().translate(plane.getNormal() * distance));
            }else
            {
                std::vector<Plane<float>> threePlanes = findThreeNonParallelPlanes(polyhedronPoint.getFaceIndices(), planes);
                Point3<float> newPoint;
                if (threePlanes.size() == 3)
                {
                    Vector3<float> n1CrossN2 = threePlanes[0].getNormal().crossProduct(threePlanes[1].getNormal());
                    Vector3<float> n2CrossN3 = threePlanes[1].getNormal().crossProduct(threePlanes[2].getNormal());
                    Vector3<float> n3CrossN1 = threePlanes[2].getNormal().crossProduct(threePlanes[0].getNormal());

                    newPoint = Point3<float>(n2CrossN3 * threePlanes[0].getDistanceToOrigin());
                    newPoint += Point3<float>(n3CrossN1 * threePlanes[1].getDistanceToOrigin());
                    newPoint += Point3<float>(n1CrossN2 * threePlanes[2].getDistanceToOrigin());
                    newPoint *= -1.0 / threePlanes[0].getNormal().dotProduct(n2CrossN3);
                } else
                { //useless point found on polyhedron (could be removed from polyhedron without impact)
                    std::stringstream logStream;
                    logStream.precision(std::numeric_limits<float>::max_digits10);
                    logStream << "Impossible to resize polyhedron because of useless point." << std::endl;
                    logStream << " - Polyhedron: " << std::endl << (*this) << std::endl;
                    Logger::logger().logError(logStream.str());
                }

				polyhedronPoint.setPoint(newPoint);
            }
		}

		for(auto &face : faces)
		{
			face.refreshWith(points);
		}
	}

	std::vector<Plane<float>> Polyhedron::buildPlanesFromFaces() const
	{
		std::vector<Plane<float>> planes;
		planes.reserve(faces.size());

		for(const auto &face : faces)
		{
			planes.emplace_back(Plane<float>(points[face.getCcwPointIndices()[0]].getPoint(), points[face.getCcwPointIndices()[1]].getPoint(), points[face.getCcwPointIndices()[2]].getPoint()));
		}

		return planes;
	}

	void Polyhedron::shiftPlanes(std::vector<Plane<float>> &planes, const NavMeshAgent &agent) const
	{
		for(auto &plane : planes)
		{
			float distance = agent.computeExpandDistance(plane.getNormal());
			plane.setDistanceToOrigin(plane.getDistanceToOrigin() - distance);
		}
	}

	std::vector<Plane<float>> Polyhedron::findThreeNonParallelPlanes(const std::vector<unsigned int> &planeIndices, const std::vector<Plane<float>> &allPlanes) const
	{
        constexpr float PARALLEL_COMPARISON_TOLERANCE = 0.01f;

		std::vector<Plane<float>> nonParallelPlanes;
		nonParallelPlanes.reserve(3);

		Plane<float> plane1 = allPlanes.at(planeIndices[0]);
		for(unsigned int i=1; i<planeIndices.size(); ++i)
		{
			Plane<float> plane2 = allPlanes.at(planeIndices[i]);
			if(plane1.getNormal().crossProduct(plane2.getNormal()).squareLength() < PARALLEL_COMPARISON_TOLERANCE)
			{ //planes are parallel: continue on next plane
				continue;
			}

			for(unsigned int j=i+1; j<planeIndices.size(); ++j)
			{
				Plane<float> plane3 = allPlanes.at(planeIndices[j]);

				Vector3<float> n2CrossN3 = plane2.getNormal().crossProduct(plane3.getNormal());
				if(n2CrossN3.squareLength() < 0.0
						|| plane3.getNormal().crossProduct(plane1.getNormal()).squareLength() < PARALLEL_COMPARISON_TOLERANCE
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

    std::ostream& operator <<(std::ostream &stream, const Polyhedron &polyhedron)
    {
        unsigned int faceIndex = 0;
        for(const auto &face : polyhedron.getFaces())
        {
            stream<<"Face "<<faceIndex++<<" ";
            for(unsigned int pointIndex : face.getCcwPointIndices())
            {
                stream<<"("<<polyhedron.getPoints()[pointIndex].getPoint()<<") ";
            }
            stream<<std::endl;
        }

        return stream;
    }
}
