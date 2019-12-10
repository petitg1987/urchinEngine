#include "NavPolygon.h"

#include <utility>

namespace urchin
{

	NavPolygon::NavPolygon(std::string name, std::vector<Point3<float>> &&points, const NavTopography *navTopography) :
        	name(std::move(name)),
			points(std::move(points)),
			navTopography(navTopography)
	{

	}

	NavPolygon::NavPolygon(const NavPolygon &navPolygon) :
			name(navPolygon.getName()),
			points(navPolygon.getPoints()),
			triangles(navPolygon.getTriangles()),
			navTopography(nullptr) //no copy because not used
	{

	}

	NavPolygon::~NavPolygon()
	{
		delete navTopography;
	}

    const std::string &NavPolygon::getName() const
    {
        return name;
    }

	const std::vector<Point3<float>> &NavPolygon::getPoints() const
	{
		return points;
	}

	const Point3<float> &NavPolygon::getPoint(unsigned int index) const
	{
		return points[index];
	}

	void NavPolygon::addTriangles(const std::vector<std::shared_ptr<NavTriangle>> &triangles, const std::shared_ptr<NavPolygon> &thisNavPolygon)
	{
		#ifdef _DEBUG
			assert(thisNavPolygon.get()==this);
        #endif

		this->triangles = triangles;
		for(const auto &triangle : triangles)
		{
			triangle->attachNavPolygon(thisNavPolygon);
		}
	}

	const std::vector<std::shared_ptr<NavTriangle>> &NavPolygon::getTriangles() const
	{
		return triangles;
	}

	const std::shared_ptr<NavTriangle> &NavPolygon::getTriangle(std::size_t index) const
	{
		return triangles[index];
	}

	const NavTopography *NavPolygon::getNavTopography() const
	{
		return navTopography;
	}

}
