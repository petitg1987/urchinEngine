#include "scene/GUI/widget/Position.h"

namespace urchin {

    Position::Position(float positionX, PositionType positionTypeX, float positionY, PositionType positionTypeY) :
            positionX(positionX),
            positionY(positionY),
            positionTypeX(positionTypeX),
            positionTypeY(positionTypeY) {

    }

    Position::Position(float positionX, float positionY, PositionType positionType) :
        Position(positionX, positionType, positionY, positionType) {

    }

    float Position::getPositionX() const {
        return positionX;
    }

    Position::PositionType Position::getPositionTypeX() const {
        return positionTypeX;
    }

    float Position::getPositionY() const {
        return positionY;
    }

    Position::PositionType Position::getPositionTypeY() const {
        return positionTypeY;
    }

}
