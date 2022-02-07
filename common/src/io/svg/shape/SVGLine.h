#pragma once

#include <string>

#include <io/svg/shape/SVGShape.h>
#include <math/geometry/2d/object/LineSegment2D.h>

namespace urchin {

    class SVGLine : public SVGShape {
        public:
            SVGLine(const LineSegment2D<float>&, SVGColor, float opacity = 1.0f);

            std::string getShapeTag() const override;
            Rectangle2D<float> computeRectangle() const override;

        private:
            LineSegment2D<float> line;
    };

}
