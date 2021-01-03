#include "Length.h"

namespace urchin {

    Length::Length(float value, LengthType type) :
            value(value),
            type(type) {

    }

    float Length::getValue() const {
        return value;
    }

    Length::LengthType Length::getType() const {
        return type;
    }

}
