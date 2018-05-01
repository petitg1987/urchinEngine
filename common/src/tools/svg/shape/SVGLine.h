#ifndef TESTENGINESFML_SVGLINE_H
#define TESTENGINESFML_SVGLINE_H

#include <string>

#include "tools/svg/shape/SVGShape.h"
#include "math/geometry/2d/object/LineSegment2D.h"

namespace urchin
{

    class SVGLine : public SVGShape
    {
        public:
            SVGLine(const LineSegment2D<float> &, SVGColor, float opacity = 1.0f);

            std::string getShapeTag() const override;
            Rectangle<float> computeRectangle() const override;

        private:
            LineSegment2D<float> line;
    };

}

#endif
