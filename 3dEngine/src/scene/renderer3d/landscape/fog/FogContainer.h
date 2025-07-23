#pragma once

#include <stack>
#include <memory>

#include "graphics/api/GraphicsApi.h"
#include "graphics/render/GenericRendererBuilder.h"
#include "scene/renderer3d/landscape/fog/Fog.h"

namespace urchin {

    class FogContainer {
        public:
            FogContainer();

            void pushFog(const Fog*);
            void popFog();
            const Fog* getActiveFog() const;

            void setupDeferredSecondPassRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t) const;

            void loadFog(GenericRenderer&, uint32_t);

        private:
            std::stack<const Fog*> fogs;
            struct FogData {
                alignas(4) bool hasFog;
                alignas(4) float density;
                alignas(4) float gradient;
                alignas(4) float maxHeight;
                alignas(16) Vector3<float> color;
            } fogData;
    };

}
