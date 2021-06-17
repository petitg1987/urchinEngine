#pragma once

#include <scene/ui/widget/LengthType.h>
#include <scene/ui/widget/Length.h>

namespace urchin {

    class Position {
        public:
            Position(float, LengthType, float, LengthType);
            Position(float, float, LengthType);

            float getPositionX() const;
            LengthType getPositionTypeX() const;

            float getPositionY() const;
            LengthType getPositionTypeY() const;

        private:
            Length positionX;
            Length positionY;
    };

}
