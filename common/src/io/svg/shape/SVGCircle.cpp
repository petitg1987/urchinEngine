#include <io/svg/shape/SVGCircle.h>

namespace urchin {

    SVGCircle::SVGCircle(const Point2<float>& center, float radius, SVGColor color, float opacity) :
            SVGShape(color, SVGShape::BLACK, 0.0f, opacity),
            center(center),
            radius(radius) {
        this->center.Y = -this->center.Y;
    }

    std::string SVGCircle::getShapeTag() const {
        std::stringstream stream;

        stream << "<circle cx=\"";
        stream << center.X;
        stream << "\" cy=\"";
        stream << center.Y;
        stream << "\"";

        stream << " r=\"";
        stream << radius;
        stream << "\"";

        stream << " style=\"" + getStyle() + "\" />";

        return stream.str();
    }

    Rectangle2D<float> SVGCircle::computeRectangle() const {
        Point2<float> minPoint(center - radius);
        Point2<float> maxPoint(center + radius);

        return {minPoint, maxPoint};
    }
}
