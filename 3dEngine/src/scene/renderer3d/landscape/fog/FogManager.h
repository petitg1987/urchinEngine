#pragma once

#include <stack>
#include <memory>

#include <scene/renderer3d/landscape/fog/Fog.h>
#include <graphic/render/GenericRenderer.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/model/Shader.h>

namespace urchin {

    class FogManager {
        public:
            FogManager();

            void pushFog(const Fog*);
            void popFog();
            const Fog* getActiveFog() const;

            void setupLightingRenderer(std::shared_ptr<GenericRendererBuilder>&);

            void loadFog(GenericRenderer&);

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
