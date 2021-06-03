#ifndef URCHINENGINE_WIDGETPOSITION_H
#define URCHINENGINE_WIDGETPOSITION_H

#include <scene/UI/widget/LengthType.h>
#include <scene/UI/widget/Length.h>

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

#endif
