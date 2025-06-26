#include <scene/renderer3d/VisualConfig.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h>

namespace urchin {

    AntiAliasingApplier::AntiAliasingApplier(const Config& config, bool isTestMode) :
            isTestMode(isTestMode),
            config(config),
            fxaaApplier(FxaaApplier(isTestMode)),
            taaApplier(TaaApplier(isTestMode)) {

    }

    void AntiAliasingApplier::updateCamera(Camera& camera) {
        if (config.useTaa) {
            taaApplier.applyCameraJitter(camera);
        }
    }

    void AntiAliasingApplier::refreshInputTexture(const std::shared_ptr<Texture>& inputTexture) {
        taaApplier.refreshInputTexture(inputTexture);
        fxaaApplier.refreshInputTexture(inputTexture);
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

    void AntiAliasingApplier::applyAntiAliasing(uint32_t frameIndex, unsigned int numDependenciesToAATexture) {
        ScopeProfiler sp(Profiler::graphic(), "applyAA");

        if (config.useTaa) {
            taaApplier.applyAntiAliasing(frameIndex, numDependenciesToAATexture);
        } else {
            fxaaApplier.applyAntiAliasing(frameIndex, numDependenciesToAATexture);
        }
    }

}
