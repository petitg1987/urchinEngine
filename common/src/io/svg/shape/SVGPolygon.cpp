#include <limits>
#include <utility>

#include <io/svg/shape/SVGPolygon.h>

namespace urchin {

    SVGPolygon::SVGPolygon(std::vector<Point2<float>> polygonPoints, SVGColor color, float opacity) :
            SVGShape(color, BLACK, 0.0f, opacity),
            polygonPoints(std::move(polygonPoints)) {

    }

    std::string SVGPolygon::getShapeTag() const {
        std::stringstream stream;
        stream << "<polygon points=\"";
        for (const auto& polygonPoint : polygonPoints) {
            stream << polygonPoint.X << "," << polygonPoint.Y << " ";
        }
        stream << "\"";

        stream << " style=\"" + getStyle() + "\" />";

        return stream.str();
    }

    Rectangle2D<float> SVGPolygon::computeRectangle() const {
        Point2 minPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point2 maxPoint(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

        for (const auto& polygonPoint : polygonPoints) {
            if (polygonPoint.X < minPoint.X) {
                minPoint.X = polygonPoint.X;
            }
            if (polygonPoint.Y < minPoint.Y) {
                minPoint.Y = polygonPoint.Y;
            }

            if (polygonPoint.X > maxPoint.X) {
                maxPoint.X = polygonPoint.X;
            }
            if (polygonPoint.Y > maxPoint.Y) {
                maxPoint.Y = polygonPoint.Y;
            }
        }

        return {minPoint, maxPoint};
    }
}
