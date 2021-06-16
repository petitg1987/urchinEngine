#include <stdexcept>

#include <scene/UI/widget/Position.h>

namespace urchin {

    Position::Position(float positionX, LengthType positionTypeX, float positionY, LengthType positionTypeY) :
            positionX(Length(positionX, positionTypeX)),
            positionY(Length(positionY, positionTypeY)) {
        if (positionTypeX == LengthType::RELATIVE_LENGTH || positionTypeY == LengthType::RELATIVE_LENGTH) {
            throw std::invalid_argument("Relative lengths for position are not implemented");
        }
    }

    Position::Position(float positionX, float positionY, LengthType positionType) :
        Position(positionX, positionType, positionY, positionType) {

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

}
