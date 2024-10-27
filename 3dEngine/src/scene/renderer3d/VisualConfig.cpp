#include <scene/renderer3d/VisualConfig.h>

namespace urchin {

    //static
    const TextureFormat VisualConfig::ALBEDO_AND_EMISSIVE_TEXTURE_FORMAT = TextureFormat::RGBA_8_INT;
    const TextureFormat VisualConfig::NORMAL_AND_AMBIENT_TEXTURE_FORMAT = TextureFormat::RGBA_8_INT;
    const TextureFormat VisualConfig::MATERIAL_TEXTURE_FORMAT = TextureFormat::RG_8_INT;
    const TextureFormat VisualConfig::SCENE_TEXTURE_FORMAT = TextureFormat::RGBA_16_FLOAT;


    VisualConfig::VisualConfig() :
            renderingScale(1.0f),
            shadowActivated(true),
            shadowConfig({}),
            ambientOcclusionActivated(true),
            ambientOcclusionConfig({}),
            antiAliasingActivated(true),
            antiAliasingConfig({}),
            bloomConfig({}),
            reflectionActivated(true),
            reflectionConfig({}),
            terrainConfig({}) {

    }

    void VisualConfig::setRenderingScale(float renderingScale) {
        this->renderingScale = renderingScale;
    }

    float VisualConfig::getRenderingScale() const {
        return renderingScale;
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

    void VisualConfig::activateReflection(bool reflectionActivated) {
        this->reflectionActivated = reflectionActivated;
    }

    bool VisualConfig::isReflectionActivated() const {
        return reflectionActivated;
    }

    void VisualConfig::setReflectionConfig(const ReflectionApplier::Config& reflectionConfig) {
        this->reflectionConfig = reflectionConfig;
    }

    const ReflectionApplier::Config& VisualConfig::getReflectionConfig() const {
        return reflectionConfig;
    }

    void VisualConfig::setTerrainConfig(const TerrainContainer::Config& terrainConfig) {
        this->terrainConfig = terrainConfig;
    }

    const TerrainContainer::Config& VisualConfig::getTerrainConfig() const {
        return terrainConfig;
    }

}
