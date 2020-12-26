#include <stdexcept>

#include "PolygonsUnion.h"

namespace urchin {

    /**
       * Perform an union of polygons.
       * When polygons cannot be put together because there is no contact: there are returned apart.
       */
    template<class T> std::vector<CSGPolygon<T>> &PolygonsUnion<T>::unionPolygons(const std::vector<CSGPolygon<T>>& polygons) const {
        mergedPolygons.clear();

        allPolygonPaths.clear();
        for (const auto& polygon : polygons) {
            allPolygonPaths.emplace_back(CSGPolygonPath(polygon));
        }

        while (!allPolygonPaths.empty()) {
            bool isPolygonsMerged = false;
            for (std::size_t i = 1; i < allPolygonPaths.size(); ++i) {
                const std::vector<CSGPolygonPath>& result = unionTwoPolygonPaths(allPolygonPaths[0], allPolygonPaths[i]);
                if (result.empty()) {
                    logInputData(polygons, "Empty result returned after two polygons union." , Logger::ERROR_LVL);
                    mergedPolygons.clear();
                    return mergedPolygons;
                }
                if (result.size() == 1) {
                    isPolygonsMerged = true;

                    VectorEraser::erase(allPolygonPaths, i);
                    VectorEraser::erase(allPolygonPaths, 0);

                    allPolygonPaths.emplace_back(result[0]);
                    break;
                }
            }
            if (!isPolygonsMerged) {
                mergedPolygons.push_back(allPolygonPaths[0].template toCSGPolygon<T>());
                VectorEraser::erase(allPolygonPaths, 0);
            }
        }

        return mergedPolygons;
    }

    template<class T> const std::vector<CSGPolygonPath> &PolygonsUnion<T>::unionTwoPolygonPaths(const CSGPolygonPath& polygon1, const CSGPolygonPath& polygon2) const {
        ClipperLib::Clipper clipper;
        clipper.ReverseSolution(true);
        clipper.StrictlySimple(true); //slow but avoid duplicate points
        clipper.AddPath(polygon1.getPath(), ClipperLib::ptSubject, true);
        clipper.AddPath(polygon2.getPath(), ClipperLib::ptClip, true);

        ClipperLib::PolyTree solution;
        clipper.Execute(ClipperLib::ctUnion, solution, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

        twoPolygonUnions.clear();

        if (solution.Childs.size() == 1) {
            assert(!solution.Childs[0]->IsOpen());
            assert(!solution.Childs[0]->IsHole());

            std::string unionName = "{" + polygon1.getName() + "} ∪ {" + polygon2.getName() + "}";
            twoPolygonUnions.emplace_back(CSGPolygonPath(solution.Childs[0]->Contour, unionName));
        } else if (solution.Childs.size() == 2) {
            twoPolygonUnions.emplace_back(CSGPolygonPath(solution.Childs[0]->Contour, polygon1.getName()));
            twoPolygonUnions.emplace_back(CSGPolygonPath(solution.Childs[1]->Contour, polygon2.getName()));
        }

        return twoPolygonUnions;
    }

    template<class T> void PolygonsUnion<T>::logInputData(const std::vector<CSGPolygon<T>>& polygons, const std::string& message,
            Logger::CriticalityLevel logLevel) const {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<T>::max_digits10);

        logStream << message << std::endl;

        for (std::size_t i = 0; i < polygons.size(); ++i) {
            logStream << "Polygon " << i << std::endl << polygons[i];
            if (i < polygons.size() - 1) {
                logStream << std::endl;
            }

        }

        Logger::instance()->log(logLevel, logStream.str());
    }

    //explicit template
    template class PolygonsUnion<float>;

    template class PolygonsUnion<long long>;
}
