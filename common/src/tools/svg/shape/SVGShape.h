#ifndef URCHINENGINE_SVGSHAPE_H
#define URCHINENGINE_SVGSHAPE_H

#include <string>

#include "math/geometry/2d/object/Rectangle.h"

namespace urchin
{

    class SVGShape
    {
        public:
            enum SVGColor
            {
                BLACK,
                LIME,
                RED,
                BLUE,
                ORANGE,
                GREEN,
                YELLOW
            };

            SVGShape(SVGColor, float);
            virtual ~SVGShape() = default;

            void setStroke(SVGColor, float);

            virtual std::string getShapeTag() const = 0;
            virtual Rectangle<float> computeRectangle() const = 0;

        protected:
            std::string getStyle() const;
            std::string toColorString(SVGColor) const;

            SVGColor color;
            float opacity;
            SVGColor strokeColor;
            float strokeSize;
    };

}

#endif
