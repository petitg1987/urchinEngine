#pragma once

#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h>
#include <scene/renderer3d/lighting/shadow/ShadowManager.h>
#include <scene/renderer3d/lighting/ambientocclusion/AmbientOcclusionManager.h>
#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <scene/renderer3d/landscape/terrain/TerrainContainer.h>

namespace urchin {

    class VisualConfig {
        public:
            VisualConfig();

            void setRenderingScale(float);
            float getRenderingScale() const;

            void activateShadow(bool);
            bool isShadowActivated() const;
            void setShadowConfig(const ShadowManager::Config&);
            const ShadowManager::Config& getShadowConfig() const;

            void activateAmbientOcclusion(bool);
            bool isAmbientOcclusionActivated() const;
            void setAmbientOcclusionConfig(const AmbientOcclusionManager::Config&);
            const AmbientOcclusionManager::Config& getAmbientOcclusionConfig() const;

            void activateAntiAliasing(bool);
            bool isAntiAliasingActivated() const;
            void setAntiAliasingConfig(const AntiAliasingApplier::Config&);
            const AntiAliasingApplier::Config& getAntiAliasingConfig() const;

            void setBloomConfig(const BloomEffectApplier::Config&);
            const BloomEffectApplier::Config& getBloomConfig() const;

            void setTerrainConfig(const TerrainContainer::Config&);
            const TerrainContainer::Config& getTerrainConfig() const;

        private:
            float renderingScale;

            bool shadowActivated;
            ShadowManager::Config shadowConfig;

            bool ambientOcclusionActivated;
            AmbientOcclusionManager::Config ambientOcclusionConfig;

            bool antiAliasingActivated;
            AntiAliasingApplier::Config antiAliasingConfig;

            BloomEffectApplier::Config bloomConfig;

            TerrainContainer::Config terrainConfig;
    };

}