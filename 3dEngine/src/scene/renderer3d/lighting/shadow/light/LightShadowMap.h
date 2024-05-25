#pragma once

#include <vector>
#include <set>
#include <memory>
#include <UrchinCommon.h>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/lighting/light/Light.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/culler/ModelOcclusionCuller.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <texture/filter/TextureFilter.h>

namespace urchin {

    class LightSplitShadowMap;
    class ShadowModelShaderVariable;

    class LightShadowMap final : public Observer {
        public:
            LightShadowMap(Light&, const ModelOcclusionCuller&, float, unsigned int, unsigned int);
            ~LightShadowMap() override;

            void notify(Observable*, int) override;

            Light& getLight() const;
            const ModelOcclusionCuller& getModelOcclusionCuller() const;
            float getViewingShadowDistance() const;
            unsigned int getNumberShadowMaps() const;
            unsigned int getShadowMapSize() const;
            const std::shared_ptr<Texture>& getShadowMapTexture() const;

            const std::vector<std::unique_ptr<LightSplitShadowMap>>& getLightSplitShadowMaps() const;

            const Matrix4<float>& getLightViewMatrix() const;
            void removeModel(Model* model) const;

            void renderModels(std::uint32_t, unsigned int, unsigned int) const;

        private:
            void updateLightViewMatrix();
            std::span<Model*> retrieveModels() const;

            Light& light;
            const ModelOcclusionCuller& modelOcclusionCuller;
            float viewingShadowDistance;
            unsigned int nbShadowMaps;

            std::unique_ptr<OffscreenRender> renderTarget;
            std::unique_ptr<ModelSetDisplayer> shadowModelSetDisplayer;

            Matrix4<float> lightViewMatrix;
            std::vector<std::unique_ptr<LightSplitShadowMap>> lightSplitShadowMaps;
            mutable std::vector<Model*> models;
    };

}
