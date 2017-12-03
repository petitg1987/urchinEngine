#include <algorithm>
#include <cmath>

#include "Polytope.h"
#include "path/navmesh/polytope/PolytopePlaneSurface.h"
#include "path/navmesh/polytope/PolytopeTerrainSurface.h"

namespace urchin
{

	/**
	 * @param surfaces Indexed faces of the polytope. Surfaces must have their points in counter-clockwise to have face normal pointing outside the polyhedron.
	 * @param points All points of the polytopes
	 */
	Polytope::Polytope(const std::string &name, std::vector<std::unique_ptr<PolytopeSurface>> &surfaces, const std::vector<PolytopePoint> &points) :
			name(name),
			surfaces(std::move(surfaces)),
			points(points),
			walkableCandidate(true),
            obstacleCandidate(true)
	{
        buildXZRectangle();
	}

	const std::string Polytope::getName() const
	{
		return name;
	}

	const std::vector<std::unique_ptr<PolytopeSurface>> &Polytope::getSurfaces() const
	{
		return surfaces;
	}

	const std::unique_ptr<PolytopeSurface> &Polytope::getSurface(unsigned int surfaceIndex) const
	{
		return surfaces[surfaceIndex];
	}

	const std::unique_ptr<Rectangle<float>> &Polytope::getXZRectangle() const
	{
		return xzRectangle;
	}

	void Polytope::setWalkableCandidate(bool walkableCandidate)
	{
		this->walkableCandidate = walkableCandidate;
	}

	bool Polytope::isWalkableCandidate() const
	{
		return walkableCandidate;
	}

    void Polytope::setObstacleCandidate(bool obstacleCandidate)
    {
        Polytope::obstacleCandidate = obstacleCandidate;
    }

    bool Polytope::isObstacleCandidate() const
    {
        return obstacleCandidate;
    }

	std::unique_ptr<Polytope> Polytope::expand(const NavMeshAgent &agent) const
	{
		if(surfaces.size() > 1)
		{ //polyhedron (3D)
			return expandPolyhedron(agent);
		}else if(surfaces.size()==1)
		{ //polygon (2D)
			//TODO expand polygon
		}
	}

	std::unique_ptr<Polytope> Polytope::expandPolyhedron(const NavMeshAgent &agent) const
	{
		//1. compute expanded points
		std::vector<PolytopePoint> expandedPoints;
		expandedPoints.reserve(points.size());

		std::vector<Plane<float>> planes = buildPlanesFromPlaneSurfaces();
		shiftPlanes(planes, agent);

		for(auto &polyhedronPoint : points)
		{
			if(polyhedronPoint.getFaceIndices().size()==1)
			{ //face is individual
				Plane<float> plane = planes[polyhedronPoint.getFaceIndices()[0]];
				float distance = agent.computeExpandDistance(plane.getNormal());
				Point3<float> newPoint = polyhedronPoint.getPoint().translate(plane.getNormal() * distance);
				expandedPoints.emplace_back(PolytopePoint(newPoint, polyhedronPoint.getFaceIndices()));
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
				{ //useless point found on polytope (could be removed from polytope without impact)
					std::stringstream logStream;
					logStream.precision(std::numeric_limits<float>::max_digits10);
					logStream << "Impossible to resize polytope because of useless point." << std::endl;
					logStream << " - Polytope: " << std::endl << (*this) << std::endl;
					Logger::logger().logError(logStream.str());
				}

				expandedPoints.emplace_back(PolytopePoint(newPoint, polyhedronPoint.getFaceIndices()));
			}
		}

		//2. compute expanded surfaces
		std::vector<std::unique_ptr<PolytopeSurface>> expandedSurfaces;
		expandedSurfaces.reserve(surfaces.size());
		for(auto &surface : surfaces)
		{
			auto *planeSurface = dynamic_cast<PolytopePlaneSurface *>(surface.get());
			expandedSurfaces.emplace_back(std::make_unique<PolytopePlaneSurface>(planeSurface->getCcwPointIndices(), expandedPoints));
		}

		auto expandedPolytope = std::make_unique<Polytope>(getName(), expandedSurfaces, expandedPoints);
        expandedPolytope->setWalkableCandidate(isWalkableCandidate());
        expandedPolytope->setObstacleCandidate(isObstacleCandidate());
        return expandedPolytope;
	}

	std::vector<Plane<float>> Polytope::buildPlanesFromPlaneSurfaces() const
	{
		std::vector<Plane<float>> planes;
		planes.reserve(surfaces.size());

		Rectangle<float> nullRectangle(Point2<float>(0.0, 0.0), Point2<float>(0.0, 0.0));
		for(const auto &surface : surfaces)
		{
			planes.emplace_back(surface->getPlaneIn(nullRectangle));
		}

		return planes;
	}

	void Polytope::shiftPlanes(std::vector<Plane<float>> &planes, const NavMeshAgent &agent) const
	{
		for(auto &plane : planes)
		{
			float distance = agent.computeExpandDistance(plane.getNormal());
			plane.setDistanceToOrigin(plane.getDistanceToOrigin() - distance);
		}
	}

	std::vector<Plane<float>> Polytope::findThreeNonParallelPlanes(const std::vector<unsigned int> &planeIndices, const std::vector<Plane<float>> &allPlanes) const
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

	void Polytope::buildXZRectangle()
	{
		Rectangle<float> xzRectangle = surfaces[0]->computeXZRectangle();
		for(unsigned int i=1; i<surfaces.size(); ++i)
		{
			xzRectangle = xzRectangle.merge(surfaces[i]->computeXZRectangle());
		}
		this->xzRectangle = std::make_unique<Rectangle<float>>(xzRectangle.getMin(), xzRectangle.getDiagonal());
	}

    std::ostream& operator <<(std::ostream &stream, const Polytope &polytope)
    {
        unsigned int surfaceIndex = 0;
        for(const auto &surface : polytope.getSurfaces())
        {
            stream<<"Surface "<<surfaceIndex++<<" ";
			if(const auto *planeSurface = dynamic_cast<PolytopePlaneSurface *>(surface.get()))
			{
				for (const auto &point : planeSurface->getCcwPoints())
				{
					stream << "(" << point << ") ";
				}
				stream << std::endl;
			}else if(const auto *terrainSurface = dynamic_cast<PolytopeTerrainSurface *>(surface.get()))
			{
				terrainSurface->isWalkableCandidate(); //@IgnoreUnused
				stream << "(terrain: " << polytope.getName() << ") " << std::endl;
			}
        }

        return stream;
    }
}
