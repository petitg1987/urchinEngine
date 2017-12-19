#ifndef TESTENGINESFML_SVGPOLYGON_H
#define TESTENGINESFML_SVGPOLYGON_H

#include <vector>
#include <string>

#include "math/algebra/point/Point2.h"
#include "math/geometry/2d/object/Rectangle.h"

namespace urchin
{

    class SVGPolygon
    {
        public:
            enum SVGColor
            {
                LIME,
                RED
            };

            SVGPolygon(const std::vector<Point2<float>> &, SVGColor color, float opacity = 1.0f);

            const std::vector<Point2<float>> &getPolygonPoints() const;
            std::string getStyle() const;

            Rectangle<int> computeRectangle() const;

        private:
            std::vector<Point2<float>> polygonPoints;
            SVGColor color;
            float opacity;
    };

}

#endif
