#include <algorithm>
#include <cmath>

#include "Polytope.h"
#include "path/navmesh/polytope/PolytopePlaneSurface.h"
#include "path/navmesh/polytope/PolytopeTerrainSurface.h"

namespace urchin
{

	/**
	 * @param surfaces Indexed faces of the polytope. Surfaces must have their points in counter-clockwise to have face normal pointing outside the polyhedron.
	 */
	Polytope::Polytope(const std::string &name, std::vector<std::unique_ptr<PolytopeSurface>> &surfaces) :
			name(name),
			surfaces(std::move(surfaces)),
			walkableCandidate(true),
            obstacleCandidate(true)
	{
        buildXZRectangle();
	}

	const std::string &Polytope::getName() const
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
