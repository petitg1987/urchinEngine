#include <map>
#include <limits>

#include "SVGPolygon.h"

namespace urchin
{
    SVGPolygon::SVGPolygon(const std::vector<Point2<float>> &polygonPoints, SVGColor color, float opacity) :
            SVGShape(color, opacity),
            polygonPoints(polygonPoints)
    {
        for(auto &polygonPoint : this->polygonPoints)
        { //SVG Y axis is up side down
            polygonPoint.Y = -polygonPoint.Y;
        }
    }

    std::string SVGPolygon::getShapeTag() const
    {
        std::stringstream stream;
        stream<<"<polygon points=\"";
        for(const auto &polygonPoint : polygonPoints)
        {
            stream<<polygonPoint.X<<","<<polygonPoint.Y<<" ";
        }
        stream<<"\"";

        stream<<" style=\"" + getStyle() + "\" />";

        return stream.str();
    }

    Rectangle<int> SVGPolygon::computeRectangle() const
    {
        Point2<float> minPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point2<float> maxPoint(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

        for(const auto &polygonPoint : polygonPoints)
        {
            if(polygonPoint.X < minPoint.X)
            {
                minPoint.X = polygonPoint.X;
            }
            if(polygonPoint.Y < minPoint.Y)
            {
                minPoint.Y = polygonPoint.Y;
            }

            if(polygonPoint.X > maxPoint.X)
            {
                maxPoint.X = polygonPoint.X;
            }
            if(polygonPoint.Y > maxPoint.Y)
            {
                maxPoint.Y = polygonPoint.Y;
            }
        }

        return Rectangle<int>(Point2<int>(static_cast<int>(minPoint.X), static_cast<int>(minPoint.Y)),
                              Point2<int>(static_cast<int>(maxPoint.X), static_cast<int>(maxPoint.Y)));
    }
}
