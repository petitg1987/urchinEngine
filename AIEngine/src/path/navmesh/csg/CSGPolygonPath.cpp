#include <utility>

#include "CSGPolygonPath.h"

namespace urchin {

    CSGPolygonPath::CSGPolygonPath(ClipperLib::Path path, std::string name) :
        path(std::move(path)),
        name(std::move(name)) {

    }

    template<class T> CSGPolygonPath::CSGPolygonPath(const CSGPolygon<T>& polygon) :
        name(polygon.getName()) {
        path.reserve(polygon.getCwPoints().size());

        for (const auto& point : polygon.getCwPoints()) {
            if (typeid(long long) == typeid(T)) {
                path.emplace_back(ClipperLib::IntPoint((long long)point.X, (long long)point.Y));
            } else {
                path.emplace_back(ClipperLib::IntPoint(Converter::toLongLong((float)point.X), Converter::toLongLong((float)point.Y)));
            }
        }
    }

    template<class T> CSGPolygon<T> CSGPolygonPath::toCSGPolygon() const {
        std::vector<Point2<T>> cwPoints;
        cwPoints.reserve(path.size());

        for (auto point : path) {
            if (typeid(long long) == typeid(T)) {
                cwPoints.emplace_back(Point2<T>((T)point.X, (T)point.Y));
            } else {
                cwPoints.emplace_back(Point2<T>((T)Converter::toFloat(point.X), (T)Converter::toFloat(point.Y)));
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
