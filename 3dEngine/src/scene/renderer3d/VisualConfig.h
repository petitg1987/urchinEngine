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
            VisualConfig(const ShadowManager::Config&, const AmbientOcclusionManager::Config&, const AntiAliasingApplier::Config&, const BloomEffectApplier::Config&, const TerrainContainer::Config&);

            const ShadowManager::Config& getShadowConfig() const;
            const AmbientOcclusionManager::Config& getAOConfig() const;
            const AntiAliasingApplier::Config& getAAConfig() const;
            const BloomEffectApplier::Config& getBloomConfig() const;
            const TerrainContainer::Config& getTerrainConfig() const;

        private:
            ShadowManager::Config shadowConfig;
            AmbientOcclusionManager::Config aoConfig;
            AntiAliasingApplier::Config aaConfig;
            BloomEffectApplier::Config bloomConfig;
            TerrainContainer::Config terrainConfig;

            //TODO add renderer3d.getCamera()->updateHorizontalFovAngle(getFieldOfView()); ?
            //TODO add renderer3d.activateAmbientOcclusion(true) / renderer3d.activateAntiAliasing(true) /  renderer3d.activateShadow(true)
    };

}