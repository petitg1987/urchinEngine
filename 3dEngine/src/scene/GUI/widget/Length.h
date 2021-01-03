#ifndef URCHINENGINETEST_LENGTH_H
#define URCHINENGINETEST_LENGTH_H

namespace urchin {

    class Length {
        public:
            enum LengthType {
                PIXEL,
                PERCENTAGE
            };

            Length(float, LengthType);

            float getValue() const;
            LengthType getType() const;

        private:
            float value;
            LengthType type;
    };

}

#endif
