#include <trigger/shape/SoundShape.h>

namespace urchin {

    SoundShape::SoundShape(float margin) :
            margin(margin) {

    }

    /**
     * @return Margin between shape trigger play event and trigger stop event
     */
    float SoundShape::getMargin() const {
        return margin;
    }

}
