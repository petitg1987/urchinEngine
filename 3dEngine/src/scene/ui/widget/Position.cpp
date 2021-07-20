#include <stdexcept>

#include <scene/ui/widget/Position.h>

namespace urchin {

    Position::Position(float positionX, LengthType positionTypeX, float positionY, LengthType positionTypeY, RelativeTo relativeTo) :
            positionX(Length(positionX, positionTypeX)),
            positionY(Length(positionY, positionTypeY)),
            relativeTo(relativeTo) {
        if (positionTypeX == LengthType::RELATIVE_LENGTH || positionTypeY == LengthType::RELATIVE_LENGTH) {
            throw std::invalid_argument("Relative lengths for position are not implemented");
        }
    }

    Position::Position(float positionX, float positionY, LengthType positionType, RelativeTo relativeTo) :
            Position(positionX, positionType, positionY, positionType, relativeTo) {

    }

    float Position::getPositionX() const {
        return positionX.getValue();
    }

    LengthType Position::getPositionTypeX() const {
        return positionX.getType();
    }

    float Position::getPositionY() const {
        return positionY.getValue();
    }

    LengthType Position::getPositionTypeY() const {
        return positionY.getType();
    }

    RelativeTo Position::getRelativeTo() const {
        return relativeTo;
    }

}
