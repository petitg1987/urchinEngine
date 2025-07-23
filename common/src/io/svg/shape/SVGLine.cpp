#include <sstream>

#include "io/svg/shape/SVGLine.h"

namespace urchin {

    SVGLine::SVGLine(const LineSegment2D<float>& line, SVGColor strokeColor, float strokeSize, float opacity) :
            SVGShape(BLACK, strokeColor, strokeSize, opacity),
            line(line) {

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
        Point2 minPoint(std::min(line.getA().X, line.getB().X), std::min(line.getA().Y, line.getB().Y));
        Point2 maxPoint(std::max(line.getA().X, line.getB().X), std::max(line.getA().Y, line.getB().Y));

        return {minPoint, maxPoint};
    }
}
