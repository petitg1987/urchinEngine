#pragma once

#include <scene/ui/widget/LengthType.h>
#include <scene/ui/widget/RelativeTo.h>
#include <scene/ui/widget/RefPoint.h>

namespace urchin {

    class Position {
        public:
            Position(float, LengthType, float, LengthType, RelativeTo = PARENT_LEFT_TOP, RefPoint = RefPoint::LEFT_TOP);
            Position(float, float, LengthType, RelativeTo = PARENT_LEFT_TOP, RefPoint = RefPoint::LEFT_TOP);

            float getX() const;
            LengthType getXType() const;

            float getY() const;
            LengthType getYType() const;

            RelativeTo getRelativeTo() const;
            RefPoint getReferencePoint() const;

        private:
            float x;
            LengthType xType;

            float y;
            LengthType yType;

            RelativeTo relativeTo;
            RefPoint referencePoint;
    };

}
