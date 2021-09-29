#include <stdexcept>

#include <scene/ui/widget/Position.h>

namespace urchin {

    Position::Position(float x, LengthType xType, float y, LengthType yType, RelativeTo relativeTo) :
            x(x),
            xType(xType),
            y(y),
            yType(yType),
            relativeTo(relativeTo) {
        if (xType == LengthType::RELATIVE_LENGTH || yType == LengthType::RELATIVE_LENGTH) {
            throw std::invalid_argument("Relative lengths for position are not implemented");
        }
    }

    Position::Position(float positionX, float positionY, LengthType positionType, RelativeTo relativeTo) :
            Position(positionX, positionType, positionY, positionType, relativeTo) {

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

    //TODO add reference: TOP_LEFT & CENTER & more ?

}
