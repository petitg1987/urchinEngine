#include <scene/renderer3d/VisualConfig.h>

namespace urchin {

    VisualConfig::VisualConfig() :
            shadowConfig({}),
            aoConfig({}),
            aaConfig({}),
            bloomConfig({}),
            terrainConfig({}) {

    }

    VisualConfig::VisualConfig(const ShadowManager::Config& shadowConfig, const AmbientOcclusionManager::Config& aoConfig, const AntiAliasingApplier::Config& aaConfig,
                               const BloomEffectApplier::Config& bloomConfig, const TerrainContainer::Config& terrainConfig) :
            shadowConfig(shadowConfig),
            aoConfig(aoConfig),
            aaConfig(aaConfig),
            bloomConfig(bloomConfig),
            terrainConfig(terrainConfig) {

    }

    const ShadowManager::Config& VisualConfig::getShadowConfig() const {
        return shadowConfig;
    }

    const AmbientOcclusionManager::Config& VisualConfig::getAOConfig() const {
        return aoConfig;
    }

    const AntiAliasingApplier::Config& VisualConfig::getAAConfig() const {
        return aaConfig;
    }

    const BloomEffectApplier::Config& VisualConfig::getBloomConfig() const {
        return bloomConfig;
    }

    const TerrainContainer::Config& VisualConfig::getTerrainConfig() const {
        return terrainConfig;
    }

}
