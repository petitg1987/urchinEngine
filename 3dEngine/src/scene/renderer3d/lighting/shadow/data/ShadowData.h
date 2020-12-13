#ifndef URCHINENGINE_SHADOWDATA_H
#define URCHINENGINE_SHADOWDATA_H

#include <vector>
#include <set>
#include <memory>
#include "UrchinCommon.h"

#include "graphic/render/target/OffscreenRender.h"
#include "scene/renderer3d/lighting/light/Light.h"
#include "scene/renderer3d/model/Model.h"
#include "texture/filter/TextureFilter.h"

namespace urchin {

    class FrustumShadowData;

    /**
    * Shadow execution data for a light
    */
    class ShadowData : public Observer{
        public:
            ShadowData(const Light*, const OctreeManager<Model>*, float);
            ~ShadowData() override;

            void notify(Observable*, int) override;

            void addFrustumShadowData();
            const Light* getLight() const;
            const OctreeManager<Model>* getModelOctreeManager() const;
            float getViewingShadowDistance() const;

            void setRenderTarget(std::unique_ptr<OffscreenRender>&&);
            const OffscreenRender* getRenderTarget() const;

            void setDepthTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture> & getDepthTexture() const;
            void setShadowMapTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getShadowMapTexture() const;

            void addTextureFilter(std::unique_ptr<TextureFilter>);
            void applyTextureFilters() const;
            const std::shared_ptr<Texture>& getFilteredShadowMapTexture() const;

            const Matrix4<float>& getLightViewMatrix() const;

            const std::vector<FrustumShadowData*>& getFrustumShadowData() const;

            unsigned int retrieveLayersToUpdate() const;
            const std::vector<Model*>& retrieveModels() const;

        private:
            void updateLightViewMatrix();

            const Light* light;
            const OctreeManager<Model>* modelOctreeManager;
            float viewingShadowDistance;

            std::shared_ptr<OffscreenRender> renderTarget; //target containing shadow map(s)
            std::shared_ptr<Texture> depthTexture; //depth texture
            std::shared_ptr<Texture> shadowMapTexture; //shadow map texture (variance shadow map)

            std::vector<std::unique_ptr<const TextureFilter>> textureFilters; //shadow map filters

            Matrix4<float> lightViewMatrix;
            std::vector<FrustumShadowData*> frustumShadowData;
            mutable std::vector<Model*> models;
    };

}

#endif
