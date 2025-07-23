#pragma once

#include "graphics/render/data/BlendFactor.h"

namespace urchin {

    class BlendFunction {
        public:
            static BlendFunction build(BlendFactor, BlendFactor, BlendFactor, BlendFactor);
            static BlendFunction buildDefault();
            static BlendFunction buildBlendDisabled();

            bool isBlendEnabled() const;
            BlendFactor getSrcColorFactor() const;
            BlendFactor getDstColorFactor() const;
            BlendFactor getSrcAlphaFactor() const;
            BlendFactor getDstAlphaFactor() const;

        private:
            BlendFunction(bool, BlendFactor, BlendFactor, BlendFactor, BlendFactor);

            bool bIsBlendEnabled;

            BlendFactor srcColorFactor;
            BlendFactor dstColorFactor;
            BlendFactor srcAlphaFactor;
            BlendFactor dstAlphaFactor;
    };

}
