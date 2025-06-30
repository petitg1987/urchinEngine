#include <scene/renderer3d/VisualConfig.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h>

namespace urchin {

    AntiAliasingApplier::AntiAliasingApplier(const Config& config, bool isTestMode) :
            isTestMode(isTestMode),
            config(config),
            fxaaApplier(FxaaApplier(isTestMode)),
            taaApplier(TaaApplier(isTestMode)) {

    }

    void AntiAliasingApplier::updateCamera(Camera& camera, unsigned int sceneWidth, unsigned int sceneHeight) {
        if (config.useTaa) {
            taaApplier.updateCamera(camera, sceneWidth, sceneHeight);
        }
    }

    void AntiAliasingApplier::refreshInputTexture(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& inputTexture) {
        taaApplier.refreshInputTexture(depthTexture, inputTexture);
        fxaaApplier.refreshInputTexture(inputTexture);
    }

    bool AntiAliasingApplier::useDepthTexture() const {
        return config.useTaa;
    }

    const std::shared_ptr<Texture>& AntiAliasingApplier::getOutputTexture() const {
        if (config.useTaa) {
            return taaApplier.getOutputTexture();
        } else {
            return fxaaApplier.getOutputTexture();
        }
    }

    void AntiAliasingApplier::updateConfig(const Config& config) {
        if (this->config.useTaa != config.useTaa ||
                this->config.quality != config.quality) {
            bool applierUpdated = this->config.useTaa != config.useTaa;
            bool qualityUpdated = this->config.quality != config.quality;

            this->config = config;

            if (qualityUpdated) {
                fxaaApplier.updateQuality(this->config.quality);
                taaApplier.updateQuality(this->config.quality);
            }
            if (applierUpdated) {
                fxaaApplier.enableApplier(!this->config.useTaa);
                taaApplier.enableApplier(this->config.useTaa);
            }
        }
    }

    const AntiAliasingApplier::Config& AntiAliasingApplier::getConfig() const {
        return config;
    }

    void AntiAliasingApplier::applyAntiAliasing(uint32_t frameIndex, unsigned int numDependenciesToAATexture, const Camera& camera) {
        ScopeProfiler sp(Profiler::graphic(), "applyAA");

        if (config.useTaa) {
            taaApplier.applyAntiAliasing(frameIndex, numDependenciesToAATexture, camera);
        } else {
            fxaaApplier.applyAntiAliasing(frameIndex, numDependenciesToAATexture);
        }
    }

}
