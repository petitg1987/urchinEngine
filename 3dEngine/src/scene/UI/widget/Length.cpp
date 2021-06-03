#include <scene/UI/widget/Length.h>

namespace urchin {

    Length::Length(float value, LengthType type) :
            value(value),
            type(type) {

    }

    float Length::getValue() const {
        return value;
    }

    LengthType Length::getType() const {
        return type;
    }

}
