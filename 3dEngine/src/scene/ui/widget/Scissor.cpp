#include <scene/ui/widget/Scissor.h>

namespace urchin {

    Scissor::Scissor(const Vector2<int>& scissorOffset, const Vector2<int>& scissorSize) :
            scissorOffset(scissorOffset),
            scissorSize(scissorSize) {

    }

    const Vector2<int>& Scissor::getScissorOffset() const {
        return scissorOffset;
    }

    const Vector2<int>& Scissor::getScissorSize() const {
        return scissorSize;
    }

}
