#pragma once

#include <string>

#include <math/geometry/2d/object/Rectangle2D.h>

namespace urchin {

    class SVGShape {
        public:
            enum SVGColor {
                BLACK,
                LIME,
                RED,
                BLUE,
                ORANGE,
                GREEN,
                YELLOW
            };

            SVGShape(SVGColor, SVGColor, float, float);
            virtual ~SVGShape() = default;

            void setStroke(SVGColor, float);

            virtual std::string getShapeTag() const = 0;
            virtual Rectangle2D<float> computeRectangle() const = 0;

        protected:
            std::string getStyle() const;
            std::string toColorString(SVGColor) const;

        private:
            SVGColor color;
            SVGColor strokeColor;
            float strokeSize;
            float opacity;
    };

}
