#include "SVGCircle.h"

namespace urchin
{
    SVGCircle::SVGCircle(const Point2<float> &center, float radius, SVGColor color, float opacity) :
            SVGShape(color, opacity),
            center(center),
            radius(radius)
    {
        this->center.Y = -this->center.Y;
    }

    std::string SVGCircle::getShapeTag() const
    {
        std::stringstream stream;

        stream<<"<circle x=\"";
        stream<<center.X;
        stream<<"\" y=\"";
        stream<<center.Y;
        stream<<"\"";

        stream<<" r=\"";
        stream<<radius;
        stream<<"\"";

        stream<<" style=\"" + getStyle() + "\" />";

        return stream.str();
    }

    Rectangle<int> SVGCircle::computeRectangle() const
    {
        Point2<float> minPoint(center - radius);
        Point2<float> maxPoint(center + radius);

        return Rectangle<int>(Point2<int>(static_cast<int>(minPoint.X), static_cast<int>(minPoint.Y)),
                              Point2<int>(static_cast<int>(maxPoint.X), static_cast<int>(maxPoint.Y)));
    }
}
