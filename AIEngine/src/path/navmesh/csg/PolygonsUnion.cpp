#include <stdexcept>

#include "PolygonsUnion.h"

namespace urchin
{

	/**
  	 * Perform an union of polygons.
  	 * When polygons cannot be put together because there is no contact: there are returned apart.
  	 */
    template<class T> std::vector<CSGPolygon<T>> PolygonsUnion<T>::unionPolygons(const std::vector<CSGPolygon<T>> &polygons) const
	{
        std::vector<CSGPolygon<T>> mergedPolygons;
        mergedPolygons.reserve(polygons.size()/2 + 1); //estimated memory size

        std::vector<CSGPolygonPath> allPolygonPaths;
        allPolygonPaths.reserve(polygons.size() + mergedPolygons.size());
        for(const auto &polygon : polygons)
        {
            allPolygonPaths.emplace_back(CSGPolygonPath(polygon));
        }

        while(!allPolygonPaths.empty())
        {
            bool isPolygonsMerged = false;
            for(unsigned int i=1; i<allPolygonPaths.size(); ++i)
            {
                std::vector<CSGPolygonPath> result = unionTwoPolygonPaths(allPolygonPaths[0], allPolygonPaths[i]);
                if(result.empty())
                {
                    logInputData(polygons, "Empty result returned after two polygons union." , Logger::ERROR);
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
                mergedPolygons.push_back(allPolygonPaths[0].template toCSGPolygon<T>());
                VectorEraser::erase(allPolygonPaths, 0);
            }
        }

        return mergedPolygons;
	}

    template<class T> std::vector<CSGPolygonPath> PolygonsUnion<T>::unionTwoPolygonPaths(const CSGPolygonPath &polygon1, const CSGPolygonPath &polygon2) const
    {
        ClipperLib::Clipper clipper;
        clipper.ReverseSolution(true);
        clipper.StrictlySimple(true); //slow but avoid duplicate points
        clipper.AddPath(polygon1.getPath(), ClipperLib::ptSubject, true);
        clipper.AddPath(polygon2.getPath(), ClipperLib::ptClip, true);

        ClipperLib::PolyTree solution;
        clipper.Execute(ClipperLib::ctUnion, solution, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

        std::vector<CSGPolygonPath> result;
        result.reserve(solution.Childs.size());

        if(solution.Childs.size()==1)
        {
            #ifdef _DEBUG
                assert(!solution.Childs[0]->IsOpen());
                assert(!solution.Childs[0]->IsHole());
            #endif

            std::string unionName = "{" + polygon1.getName() + "} ∪ {" + polygon2.getName() + "}";
            result.emplace_back(CSGPolygonPath(solution.Childs[0]->Contour, unionName));
        }else if(solution.Childs.size()==2)
        {
            result.emplace_back(CSGPolygonPath(solution.Childs[0]->Contour, polygon1.getName()));
            result.emplace_back(CSGPolygonPath(solution.Childs[1]->Contour, polygon2.getName()));
        }

        return result;
    }

    template<class T> void PolygonsUnion<T>::logInputData(const std::vector<CSGPolygon<T>> &polygons, const std::string &message,
            Logger::CriticalityLevel logLevel) const
    {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<T>::max_digits10);

        logStream<<message<<std::endl;

        for(unsigned int i=0; i<polygons.size(); ++i)
        {
            logStream << "Polygon " << i << std::endl << polygons[i];
            if(i < polygons.size() - 1)
            {
                logStream << std::endl;
            }

        }

        Logger::logger().log(logLevel, logStream.str());
    }

    //explicit template
    template class PolygonsUnion<float>;

	template class PolygonsUnion<long long>;
}
