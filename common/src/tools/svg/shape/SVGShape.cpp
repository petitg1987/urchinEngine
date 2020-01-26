#include "SVGShape.h"

namespace urchin
{

    SVGShape::SVGShape(SVGColor color, float opacity) :
        color(color),
        opacity(opacity),
        strokeColor(SVGShape::BLACK),
        strokeSize(0)
    {

    }

    void SVGShape::setStroke(SVGColor strokeColor, float strokeSize)
    {
        this->strokeColor = strokeColor;
        this->strokeSize = strokeSize;
    }

    std::string SVGShape::getStyle() const
    {
        return "fill:" + toColorString(color) + ";" +
               "stroke:" + toColorString(strokeColor) + ";stroke-width:"+ std::to_string(strokeSize) +";" +
               "opacity:" + std::to_string(opacity);
    }

    std::string SVGShape::toColorString(SVGColor color) const
    {
        switch(color)
        {
            case SVGColor::BLACK:
                return "black";
            case SVGColor::LIME:
                return "lime";
            case SVGColor::RED:
                return "red";
            case SVGColor::BLUE:
                return "blue";
            case SVGColor::ORANGE:
                return "orange";
            case SVGColor::GREEN:
                return "green";
            case SVGColor::YELLOW:
                return "yellow";
            default:
                throw std::runtime_error("Unknown SVG color: " + std::to_string(color));
        }
    }
}
