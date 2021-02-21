#ifndef URCHINENGINE_LIGHTSHADOWMAP_H
#define URCHINENGINE_LIGHTSHADOWMAP_H

#include <vector>
#include <set>
#include <memory>
#include "UrchinCommon.h"

#include "graphic/render/target/OffscreenRender.h"
#include "scene/renderer3d/lighting/light/Light.h"
#include "scene/renderer3d/model/Model.h"
#include "texture/filter/TextureFilter.h"

namespace urchin {

    class LightSplitShadowMap;

    class LightShadowMap : public Observer {
        public:
            LightShadowMap(const Light*, const OctreeManager<Model>*, float);
            ~LightShadowMap() override;

            void notify(Observable*, int) override;

            const Light* getLight() const;
            const OctreeManager<Model>* getModelOctreeManager() const;
            float getViewingShadowDistance() const;

            LightSplitShadowMap* addLightSplitShadowMap();
            const std::vector<LightSplitShadowMap*>& getLightSplitShadowMaps() const;

            void setRenderTarget(std::unique_ptr<OffscreenRender>&&);
            const OffscreenRender* getRenderTarget() const;

            void setShadowMapTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getShadowMapTexture() const;

            void addTextureFilter(std::unique_ptr<TextureFilter>);
            void applyTextureFilters() const;
            const std::shared_ptr<Texture>& getFilteredShadowMapTexture() const;

            const Matrix4<float>& getLightViewMatrix() const;
            unsigned int retrieveLayersToUpdate() const;
            const std::vector<Model*>& retrieveModels() const;

        private:
            void updateLightViewMatrix();

            const Light* light;
            const OctreeManager<Model>* modelOctreeManager;
            float viewingShadowDistance;

            std::shared_ptr<OffscreenRender> renderTarget; //target containing shadow map(s)
            std::shared_ptr<Texture> shadowMapTexture; //shadow map texture (variance shadow map)

            std::vector<std::unique_ptr<const TextureFilter>> textureFilters; //shadow map filters

            Matrix4<float> lightViewMatrix;
            std::vector<LightSplitShadowMap*> lightSplitShadowMaps;
            mutable std::vector<Model*> models;
    };

}

#endif
