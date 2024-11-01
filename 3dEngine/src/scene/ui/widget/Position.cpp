#include <stdexcept>

#include <scene/ui/widget/Position.h>

namespace urchin {

    Position::Position(float x, LengthType xType, float y, LengthType yType, RelativeTo relativeTo, RefPoint referencePoint) :
            x(x),
            xType(xType),
            y(y),
            yType(yType),
            relativeTo(relativeTo),
            referencePoint(referencePoint) {
        if (xType == RATIO_TO_HEIGHT || xType == RATIO_TO_WIDTH  || yType == RATIO_TO_HEIGHT || yType == RATIO_TO_WIDTH) {
            throw std::invalid_argument("Ratio lengths for position are not implemented");
        }
    }

    Position::Position(float positionX, float positionY, LengthType positionType, RelativeTo relativeTo, RefPoint referencePoint) :
            Position(positionX, positionType, positionY, positionType, relativeTo, referencePoint) {

    }

    float Position::getX() const {
        return x;
    }

    LengthType Position::getXType() const {
        return xType;
    }

    float Position::getY() const {
        return y;
    }

    LengthType Position::getYType() const {
        return yType;
    }

    RelativeTo Position::getRelativeTo() const {
        return relativeTo;
    }

    RefPoint Position::getReferencePoint() const {
        return referencePoint;
    }
}
