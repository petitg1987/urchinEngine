#include <io/svg/shape/SVGLine.h>

namespace urchin {
    SVGLine::SVGLine(const LineSegment2D<float>& line, SVGColor color, float opacity) :
            SVGShape(color, opacity),
            line(LineSegment2D<float>(Point2<float>(), Point2<float>())) {
        this->line = LineSegment2D<float>(Point2<float>(line.getA().X, -line.getA().Y), Point2<float>(line.getB().X, -line.getB().Y));
    }

    std::string SVGLine::getShapeTag() const {
        std::stringstream stream;

        stream << "<line x1=\"";
        stream << line.getA().X;
        stream << "\" y1=\"";
        stream << line.getA().Y;
        stream << "\"";

        stream << " x2=\"";
        stream << line.getB().X;
        stream << "\" y2=\"";
        stream << line.getB().Y;
        stream << "\"";

        stream << " style=\"" + getStyle() + "\" />";

        return stream.str();
    }

    Rectangle2D<float> SVGLine::computeRectangle() const {
        Point2<float> minPoint(std::min(line.getA().X, line.getB().X), std::min(line.getA().Y, line.getB().Y));
        Point2<float> maxPoint(std::max(line.getA().X, line.getB().X), std::max(line.getA().Y, line.getB().Y));

        return Rectangle2D<float>(minPoint, maxPoint);
    }
}
