#pragma once

#include <scene/ui/widget/LengthType.h>
#include <scene/ui/widget/RelativeTo.h>
#include <scene/ui/widget/RefPoint.h>

namespace urchin {

    class Position {
        public:
            Position(float, LengthType, float, LengthType, RelativeTo = RelativeTo::PARENT_TOP_LEFT, RefPoint = RefPoint::TOP_LEFT);
            Position(float, float, LengthType, RelativeTo = RelativeTo::PARENT_TOP_LEFT, RefPoint = RefPoint::TOP_LEFT);

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
