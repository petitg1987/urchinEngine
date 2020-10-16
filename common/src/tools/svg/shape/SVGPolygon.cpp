#include <map>
#include <limits>
#include <utility>

#include "SVGPolygon.h"

namespace urchin {
    SVGPolygon::SVGPolygon(std::vector<Point2<float>> polygonPoints, SVGColor color, float opacity) :
            SVGShape(color, opacity),
            polygonPoints(std::move(polygonPoints)) {
        for(auto &polygonPoint : this->polygonPoints) { //SVG Y axis is up side down
            polygonPoint.Y = -polygonPoint.Y;
        }
    }

    std::string SVGPolygon::getShapeTag() const {
        std::stringstream stream;
        stream<<"<polygon points=\"";
        for(const auto &polygonPoint : polygonPoints) {
            stream<<polygonPoint.X<<","<<polygonPoint.Y<<" ";
        }
        stream<<"\"";

        stream<<" style=\"" + getStyle() + "\" />";

        return stream.str();
    }

    Rectangle<float> SVGPolygon::computeRectangle() const {
        Point2<float> minPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point2<float> maxPoint(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

        for(const auto &polygonPoint : polygonPoints) {
            if(polygonPoint.X < minPoint.X) {
                minPoint.X = polygonPoint.X;
            }
            if(polygonPoint.Y < minPoint.Y) {
                minPoint.Y = polygonPoint.Y;
            }

            if(polygonPoint.X > maxPoint.X) {
                maxPoint.X = polygonPoint.X;
            }
            if(polygonPoint.Y > maxPoint.Y) {
                maxPoint.Y = polygonPoint.Y;
            }
        }

        return Rectangle<float>(minPoint, maxPoint);
    }
}
