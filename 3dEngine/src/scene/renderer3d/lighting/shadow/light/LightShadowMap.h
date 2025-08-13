#pragma once

#include <vector>
#include <memory>

#include "graphics/api/GraphicsApi.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"
#include "scene/renderer3d/lighting/light/Light.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/culler/ModelOcclusionCuller.h"
#include "scene/renderer3d/model/displayer/ModelSetDisplayer.h"

namespace urchin {

    class ShadowModelShaderVariable;

    class LightShadowMap final {
        public:
            LightShadowMap(bool, Light&, const ModelOcclusionCuller&, float, unsigned int, unsigned int);

            void onLightAffectedZoneUpdated() const;
            void onSplitFrustumUpdated(const std::vector<SplitFrustum>&) const;

            Light& getLight() const;
            const ModelOcclusionCuller& getModelOcclusionCuller() const;
            float getShadowViewDistance() const;
            unsigned int getNumberSplitShadowMaps() const;
            unsigned int getShadowMapSize() const;
            const std::shared_ptr<Texture>& getShadowMapTexture() const;

            const std::vector<std::unique_ptr<LightSplitShadowMap>>& getLightSplitShadowMaps() const;

            void removeModel(Model* model) const;
            void updateVisibleModels();

            void renderModels(uint32_t, unsigned int, unsigned int) const;

        private:
            Light& light;
            const ModelOcclusionCuller& modelOcclusionCuller;
            float shadowViewDistance;
            unsigned int shadowMapResolution;

            std::vector<std::unique_ptr<LightSplitShadowMap>> lightSplitShadowMaps;
            std::unique_ptr<Model> defaultEmptyModel;
            EverGrowHashMap<Model*, std::bitset<8>> modelsToLayersMask;

            std::unique_ptr<OffscreenRender> renderTarget;
            std::unique_ptr<ModelSetDisplayer> shadowModelSetDisplayer;
    };

}
