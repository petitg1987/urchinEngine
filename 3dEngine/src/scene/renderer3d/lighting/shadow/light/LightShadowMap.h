#pragma once

#include <vector>
#include <set>
#include <memory>
#include <UrchinCommon.h>

#include <api/render/target/OffscreenRender.h>
#include <scene/renderer3d/lighting/light/Light.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <texture/filter/TextureFilter.h>

namespace urchin {

    class LightSplitShadowMap;
    class ShadowModelShaderVariable;

    class LightShadowMap : public Observer {
        public:
            LightShadowMap(Light&, const OctreeManager<Model>&, float, std::shared_ptr<Texture>, unsigned int, std::unique_ptr<OffscreenRender>);
            ~LightShadowMap() override;

            void notify(Observable*, int) override;

            const Light& getLight() const;
            const OctreeManager<Model>& getModelOctreeManager() const;
            float getViewingShadowDistance() const;
            unsigned int getNumberShadowMaps() const;

            LightSplitShadowMap& addLightSplitShadowMap();
            const std::vector<std::unique_ptr<LightSplitShadowMap>>& getLightSplitShadowMaps() const;

            void addTextureFilter(std::unique_ptr<TextureFilter>);
            bool hasTextureFilter() const;
            void applyTextureFilters(std::uint64_t, unsigned int) const;
            const std::shared_ptr<Texture>& getFilteredShadowMapTexture() const;

            const Matrix4<float>& getLightViewMatrix() const;
            bool needShadowMapUpdate() const;
            void removeModel(Model* model) const;

            void renderModels(std::uint64_t, unsigned int, unsigned int) const;

        private:
            void createOrUpdateShadowModelSetDisplayer(unsigned int);

            void updateLightViewMatrix();
            std::span<Model*> retrieveModels() const;

            Light& light;
            const OctreeManager<Model>& modelOctreeManager;
            float viewingShadowDistance;
            unsigned int nbShadowMaps;

            std::unique_ptr<OffscreenRender> renderTarget; //target containing shadow map(s)
            std::unique_ptr<ModelSetDisplayer> shadowModelSetDisplayer;
            std::shared_ptr<Texture> shadowMapTexture; //shadow map texture (variance shadow map)
            std::vector<std::unique_ptr<const TextureFilter>> textureFilters; //shadow map filters

            Matrix4<float> lightViewMatrix;
            std::vector<std::unique_ptr<LightSplitShadowMap>> lightSplitShadowMaps;
            mutable std::vector<Model*> models;
    };

}
