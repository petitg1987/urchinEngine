#include <stdexcept>

#include "PolygonsUnion.h"

namespace urchin
{

    PolygonPath::PolygonPath(const ClipperLib::Path &path, const std::string &name) :
        path(path),
        name(name)
    {

    }

	/**
  	 * Perform an union of polygons.
  	 * When polygons cannot be put together because there is no contact: there are returned apart.
  	 */
    template<class T> std::vector<CSGPolygon<T>> PolygonsUnion<T>::unionPolygons(const std::vector<CSGPolygon<T>> &polygons) const
	{
        std::vector<CSGPolygon<T>> mergedPolygons;
        mergedPolygons.reserve(polygons.size()/2 + 1); //estimated memory size

        std::vector<PolygonPath> allPolygonPaths;
        allPolygonPaths.reserve(polygons.size() + mergedPolygons.size());
        for(const auto &polygon : polygons)
        {
            allPolygonPaths.emplace_back(PolygonPath(toPath(polygon), polygon.getName()));
        }

        while(!allPolygonPaths.empty())
        {
            bool isPolygonsMerged = false;
            for(unsigned int i=1; i<allPolygonPaths.size(); ++i)
            {
                std::vector<PolygonPath> result = unionTwoPolygonPaths(allPolygonPaths[0], allPolygonPaths[i]);
                if(result.empty())
                { //error: returns empty std::vector
                    return {};
                }
                if(result.size()==1)
                {
                    isPolygonsMerged = true;

                    VectorEraser::erase(allPolygonPaths, i);
                    VectorEraser::erase(allPolygonPaths, 0);

                    allPolygonPaths.emplace_back(result[0]);
                    break;
                }
            }
            if(!isPolygonsMerged)
            {
                mergedPolygons.push_back(toPolygon(allPolygonPaths[0].path, allPolygonPaths[0].name));
                VectorEraser::erase(allPolygonPaths, 0);
            }
        }

        return mergedPolygons;
	}

    template<class T> std::vector<PolygonPath> PolygonsUnion<T>::unionTwoPolygonPaths(const PolygonPath &polygon1, const PolygonPath &polygon2) const
    {
        ClipperLib::Clipper clipper;
        clipper.ReverseSolution(true);
        clipper.AddPath(polygon1.path, ClipperLib::ptClip, true);
        clipper.AddPath(polygon2.path, ClipperLib::ptClip, true);

        ClipperLib::Paths solution;
        clipper.Execute(ClipperLib::ctUnion, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

        std::vector<PolygonPath> result;
        result.reserve(solution.size());

        if(solution.size()==1)
        {
            std::string unionName = "{" + polygon1.name + "} ∪ {" + polygon2.name + "}";
            result.emplace_back(PolygonPath(solution[0], unionName));
        }else if(solution.size()==2)
        {
            result.emplace_back(PolygonPath(solution[0], polygon1.name));
            result.emplace_back(PolygonPath(solution[1], polygon2.name));
        }

        return result;
    }

	template<class T> ClipperLib::Path PolygonsUnion<T>::toPath(const CSGPolygon<T> &polygon) const
	{
		ClipperLib::Path path;
        path.reserve(polygon.getCwPoints().size());

		for(const auto &point : polygon.getCwPoints())
		{
            if(typeid(long long)==typeid(T))
            {
                path.emplace_back(ClipperLib::IntPoint(point.X, point.Y));
            }else
            {
                path.emplace_back(ClipperLib::IntPoint(Converter::toLongLong(point.X), Converter::toLongLong(point.Y)));
            }
		}

		return path;
	}

    template<class T> CSGPolygon<T> PolygonsUnion<T>::toPolygon(const ClipperLib::Path &path, const std::string &name) const
    {
        std::vector<Point2<T>> cwPoints;
        cwPoints.reserve(path.size());

        for (auto point : path)
        {
            if(typeid(long long)==typeid(T))
            {
                cwPoints.emplace_back(Point2<T>(point.X, point.Y));
            }else
            {
                cwPoints.emplace_back(Point2<T>(Converter::toFloat(point.X), Converter::toFloat(point.Y)));
            }
        }

        return CSGPolygon<T>(name, cwPoints);
    }

    //explicit template
    template class PolygonsUnion<float>;

	template class PolygonsUnion<long long>;
}
