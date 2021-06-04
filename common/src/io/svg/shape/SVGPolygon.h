#pragma once

#include <vector>
#include <string>

#include <io/svg/shape/SVGShape.h>
#include <math/algebra/point/Point2.h>

namespace urchin {

    class SVGPolygon : public SVGShape {
        public:
            SVGPolygon(std::vector<Point2<float>>, SVGColor, float opacity = 1.0f);

            std::string getShapeTag() const override;
            Rectangle<float> computeRectangle() const override;

        private:
            std::vector<Point2<float>> polygonPoints;
    };

}
