#ifndef URCHINENGINE_SHADOWDATA_H
#define URCHINENGINE_SHADOWDATA_H

#include <vector>
#include <set>
#include <memory>
#include "UrchinCommon.h"

#include "FrustumShadowData.h"
#include "graphic/render/target/OffscreenRenderer.h"
#include "scene/renderer3d/lighting/light/Light.h"
#include "texture/filter/TextureFilter.h"

namespace urchin {

    /**
    * Shadow execution data for a light
    */
    class ShadowData {
        public:
            ShadowData(const Light *, unsigned int);
            ~ShadowData();

            void setRenderTarget(std::unique_ptr<OffscreenRenderer> &&);
            const OffscreenRenderer *getRenderTarget() const;

            void setDepthTexture(const std::shared_ptr<Texture> &);
            const std::shared_ptr<Texture> & getDepthTexture() const;
            void setShadowMapTexture(const std::shared_ptr<Texture> &);
            const std::shared_ptr<Texture> &getShadowMapTexture() const;

            void addTextureFilter(std::unique_ptr<TextureFilter>);
            void applyTextureFilters() const;
            const std::shared_ptr<Texture> &getFilteredShadowMapTexture() const;

            void setLightViewMatrix(const Matrix4<float> &);
            const Matrix4<float> &getLightViewMatrix() const;

            unsigned int getNbFrustumShadowData() const;
            FrustumShadowData *getFrustumShadowData(std::size_t);
            const FrustumShadowData *getFrustumShadowData(std::size_t) const;

            const std::vector<Model *> &retrieveModels() const;

        private:
            const Light *const light;

            std::shared_ptr<OffscreenRenderer> renderTarget; //target containing shadow map(s)
            std::shared_ptr<Texture> depthTexture; //depth texture
            std::shared_ptr<Texture> shadowMapTexture; //shadow map texture (variance shadow map)

            std::vector<std::unique_ptr<const TextureFilter>> textureFilters; //shadow map filters

            Matrix4<float> lightViewMatrix;
            std::vector<FrustumShadowData *> frustumShadowData;
            mutable std::vector<Model *> models;
    };

}

#endif
