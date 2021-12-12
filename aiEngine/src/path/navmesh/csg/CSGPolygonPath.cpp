#include <utility>

#include <path/navmesh/csg/CSGPolygonPath.h>

namespace urchin {

    CSGPolygonPath::CSGPolygonPath(ClipperLib::Path path, std::string name) :
            path(std::move(path)),
            name(std::move(name)) {

    }

    template<class T> CSGPolygonPath::CSGPolygonPath(const CSGPolygon<T>& polygon) :
            name(polygon.getName()) {
        path.reserve(polygon.getCwPoints().size());

        for (const auto& point : polygon.getCwPoints()) {
            if constexpr (std::is_same_v<T, long long>) {
                path.emplace_back(ClipperLib::IntPoint((long long)point.X, (long long)point.Y));
            } else {
                path.emplace_back(ClipperLib::IntPoint(TypeConverter::toLongLong((float)point.X), TypeConverter::toLongLong((float)point.Y)));
            }
        }
    }

    template<class T> CSGPolygon<T> CSGPolygonPath::toCSGPolygon() const {
        std::vector<Point2<T>> cwPoints;
        cwPoints.reserve(path.size());

        for (auto point : path) {
            if constexpr (std::is_same_v<T, long long>) {
                cwPoints.emplace_back(Point2<T>((T)point.X, (T)point.Y));
            } else {
                cwPoints.emplace_back(Point2<T>((T)TypeConverter::toFloat(point.X), (T)TypeConverter::toFloat(point.Y)));
            }
        }

        return CSGPolygon<T>(name, std::move(cwPoints));
    }

    const ClipperLib::Path& CSGPolygonPath::getPath() const {
        return path;
    }

    const std::string& CSGPolygonPath::getName() const {
        return name;
    }

    //explicit template
    template CSGPolygonPath::CSGPolygonPath(const CSGPolygon<float>&);
    template class CSGPolygon<float> CSGPolygonPath::toCSGPolygon<float>() const;

    template CSGPolygonPath::CSGPolygonPath(const CSGPolygon<long long>&);
    template class CSGPolygon<long long> CSGPolygonPath::toCSGPolygon<long long>() const;

}
