#pragma once

#include <scene/ui/widget/LengthType.h>
#include <scene/ui/widget/Length.h>
#include <scene/ui/widget/RelativeTo.h>

namespace urchin {

    class Position {
        public:
            Position(float, LengthType, float, LengthType, RelativeTo = RelativeTo::PARENT_TOP_LEFT);
            Position(float, float, LengthType, RelativeTo = RelativeTo::PARENT_TOP_LEFT);

            float getPositionX() const;
            LengthType getPositionTypeX() const;

            float getPositionY() const;
            LengthType getPositionTypeY() const;

            RelativeTo getRelativeTo() const;

        private:
            Length positionX;
            Length positionY;
            RelativeTo relativeTo;
    };

}
