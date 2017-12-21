#ifndef TESTENGINESFML_SVGCIRCLE_H
#define TESTENGINESFML_SVGCIRCLE_H

#include <string>

#include "tools/svg/shape/SVGShape.h"

namespace urchin
{

    class SVGCircle : public SVGShape
    {
        public:
            SVGCircle(const Point2<float> &, float, SVGColor, float opacity = 1.0f);

            std::string getShapeTag() const override;
            Rectangle<int> computeRectangle() const override;

        private:
            Point2<float> center;
            float radius;
    };

}

#endif
