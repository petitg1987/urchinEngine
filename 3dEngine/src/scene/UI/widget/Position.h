#ifndef URCHINENGINE_WIDGETPOSITION_H
#define URCHINENGINE_WIDGETPOSITION_H

namespace urchin {

    class Position {
        public:
            enum PositionType { //TODO use LengthType
                PIXEL,
                PERCENTAGE
            };

            Position(float, PositionType, float, PositionType);
            Position(float, float, PositionType);

            float getPositionX() const;
            PositionType getPositionTypeX() const;

            float getPositionY() const;
            PositionType getPositionTypeY() const;

        private:
            float positionX, positionY;
            PositionType positionTypeX, positionTypeY;
    };

}

#endif
