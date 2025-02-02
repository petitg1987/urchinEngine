#include <stdexcept>

#include <io/svg/shape/SVGShape.h>

namespace urchin {

    SVGShape::SVGShape(SVGColor color, SVGColor strokeColor, float strokeSize, float opacity) :
            color(color),
            strokeColor(strokeColor),
            strokeSize(strokeSize),
            opacity(opacity) {

    }

    void SVGShape::setStroke(SVGColor strokeColor, float strokeSize) {
        this->strokeColor = strokeColor;
        this->strokeSize = strokeSize;
    }

    std::string SVGShape::getStyle() const {
        return "fill:" + toColorString(color) + ";" +
               "stroke:" + toColorString(strokeColor) + ";stroke-width:"+ std::to_string(strokeSize) +";" +
               "opacity:" + std::to_string(opacity);
    }

    std::string SVGShape::toColorString(SVGColor color) const {
        switch(color) {
            case BLACK:
                return "black";
            case LIME:
                return "lime";
            case RED:
                return "red";
            case BLUE:
                return "blue";
            case ORANGE:
                return "orange";
            case GREEN:
                return "green";
            case YELLOW:
                return "yellow";
            default:
                throw std::runtime_error("Unknown SVG color: " + std::to_string(color));
        }
    }
}
