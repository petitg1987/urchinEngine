#include <path/navmesh/csg/PolygonsSubtraction.h>
#include <path/navmesh/csg/CSGPolygonPath.h>

namespace urchin {

    template<class T> PolygonsSubtraction<T>& PolygonsSubtraction<T>::instance() {
        static PolygonsSubtraction<T> instance;
        return instance;
    }

    template<class T> const std::vector<CSGPolygon<T>> &PolygonsSubtraction<T>::subtractPolygons(const CSGPolygon<T>& minuendPolygon, const CSGPolygon<T>& subtrahendPolygon) const {
        bool subtrahendInside;
        return subtractPolygons(minuendPolygon, subtrahendPolygon, subtrahendInside);
    }

    /**
     * Perform a subtraction of polygons.
     * When subtrahendPolygon is totally included in minuendPolygon: the original minuendPolygon is returned without hole.
     * @param subtrahendInside True when subtrahendPolygon is totally included in minuendPolygon.
     */
    template<class T> const std::vector<CSGPolygon<T>> &PolygonsSubtraction<T>::subtractPolygons(const CSGPolygon<T>& minuendPolygon, const CSGPolygon<T>& subtrahendPolygon, bool& subtrahendInside) const {
        subtractedPolygons.clear();

        CSGPolygonPath minuendPolygonPath(minuendPolygon);
        CSGPolygonPath subtrahendPolygonPath(subtrahendPolygon);

        ClipperLib::Clipper clipper;
        clipper.ReverseSolution(true);
        clipper.StrictlySimple(true); //slow but avoid duplicate points
        clipper.AddPath(minuendPolygonPath.getPath(), ClipperLib::ptSubject, true);
        clipper.AddPath(subtrahendPolygonPath.getPath(), ClipperLib::ptClip, true);

        ClipperLib::PolyTree solution;
        clipper.Execute(ClipperLib::ctDifference, solution, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

        subtrahendInside = false;

        if (solution.Childs.size() == 1) {
            const auto& mainSolution = solution.Childs[0];

            if (mainSolution->Childs.empty()) {
                std::string subtractionName = "[" + minuendPolygon.getName() + "] - [" + subtrahendPolygon.getName()+ "]";
                subtractedPolygons.emplace_back(CSGPolygonPath(mainSolution->Contour, subtractionName).toCSGPolygon<T>());
            } else {
                assert(mainSolution->Childs[0]->IsHole());

                subtrahendInside = true;
                subtractedPolygons.emplace_back(minuendPolygon);
            }
        } else if (solution.Childs.size() > 1) {
            for (std::size_t i = 0; i < solution.Childs.size(); ++i) {
                std::string subtractionName = "[" + minuendPolygon.getName() + "] - [" + subtrahendPolygon.getName()+ "]{" + std::to_string(i) + "}";
                subtractedPolygons.emplace_back(CSGPolygonPath(solution.Childs[i]->Contour, subtractionName).toCSGPolygon<T>());
            }
        }

        return subtractedPolygons;
    }

    //explicit template
    template class PolygonsSubtraction<float>;

    template class PolygonsSubtraction<long long>;
}
