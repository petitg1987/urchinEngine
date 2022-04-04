#include <scene/renderer3d/VisualConfig.h>

namespace urchin {

    VisualConfig::VisualConfig() :
            shadowActivated(true),
            shadowConfig({}),
            ambientOcclusionActivated(true),
            ambientOcclusionConfig({}),
            antiAliasingActivated(true),
            antiAliasingConfig({}),
            bloomConfig({}),
            terrainConfig({}) {

    }

    void VisualConfig::activateShadow(bool shadowActivated) {
        this->shadowActivated = shadowActivated;
    }

    bool VisualConfig::isShadowActivated() const {
        return shadowActivated;
    }

    void VisualConfig::setShadowConfig(const ShadowManager::Config& shadowConfig) {
        this->shadowConfig = shadowConfig;
    }

    const ShadowManager::Config& VisualConfig::getShadowConfig() const {
        return shadowConfig;
    }

    void VisualConfig::activateAmbientOcclusion(bool ambientOcclusionActivated) {
        this->ambientOcclusionActivated = ambientOcclusionActivated;
    }

    bool VisualConfig::isAmbientOcclusionActivated() const {
        return ambientOcclusionActivated;
    }

    void VisualConfig::setAmbientOcclusionConfig(const AmbientOcclusionManager::Config& ambientOcclusionConfig) {
        this->ambientOcclusionConfig = ambientOcclusionConfig;
    }

    const AmbientOcclusionManager::Config& VisualConfig::getAmbientOcclusionConfig() const {
        return ambientOcclusionConfig;
    }

    void VisualConfig::activateAntiAliasing(bool antiAliasingActivated) {
        this->antiAliasingActivated = antiAliasingActivated;
    }

    bool VisualConfig::isAntiAliasingActivated() const {
        return antiAliasingActivated;
    }

    void VisualConfig::setAntiAliasingConfig(const AntiAliasingApplier::Config& antiAliasingConfig) {
        this->antiAliasingConfig = antiAliasingConfig;
    }

    const AntiAliasingApplier::Config& VisualConfig::getAntiAliasingConfig() const {
        return antiAliasingConfig;
    }

    void VisualConfig::setBloomConfig(const BloomEffectApplier::Config& bloomConfig) {
        this->bloomConfig = bloomConfig;
    }

    const BloomEffectApplier::Config& VisualConfig::getBloomConfig() const {
        return bloomConfig;
    }

    void VisualConfig::setTerrainConfig(const TerrainContainer::Config& terrainConfig) {
        this->terrainConfig = terrainConfig;
    }

    const TerrainContainer::Config& VisualConfig::getTerrainConfig() const {
        return terrainConfig;
    }

}
