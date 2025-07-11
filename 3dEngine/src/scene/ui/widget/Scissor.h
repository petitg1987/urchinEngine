#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class Scissor {
        public:
            Scissor(const Vector2<int>&, const Vector2<int>&);

            const Vector2<int>& getScissorOffset() const;
            const Vector2<int>& getScissorSize() const;

        private:
            Vector2<int> scissorOffset;
            Vector2<int> scissorSize;
    };

}
