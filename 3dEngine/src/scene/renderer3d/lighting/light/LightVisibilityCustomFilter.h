#pragma once

#include "scene/renderer3d/lighting/light/Light.h"

namespace urchin {

    class LightVisibilityCustomFilter {
        public:
            virtual ~LightVisibilityCustomFilter() = default;

            virtual bool isVisible(const Light&) const = 0;
    };

}
