#pragma once

#include <stack>
#include <memory>

#include <scene/renderer3d/landscape/fog/Fog.h>
#include <api/render/GenericRenderer.h>
#include <api/render/GenericRendererBuilder.h>
#include <api/render/shader/model/Shader.h>

namespace urchin {

    class FogContainer {
        public:
            FogContainer();

            void pushFog(const Fog*);
            void popFog();
            const Fog* getActiveFog() const;

            void setupLightingRenderer(std::shared_ptr<GenericRendererBuilder>&);

            void loadFog(GenericRenderer&, std::size_t);

        private:
            std::stack<const Fog*> fogs;
            struct {
                alignas(4) bool hasFog;
                alignas(4) float density;
                alignas(4) float gradient;
                alignas(4) float maxHeight;
                alignas(16) Vector4<float> color;
            } fogData;
    };

}
