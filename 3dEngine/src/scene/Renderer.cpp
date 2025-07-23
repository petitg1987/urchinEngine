#include "scene/Renderer.h"

namespace urchin {

    Renderer::Renderer(float gammaFactor) :
            gammaFactor(gammaFactor) {

    }

    void Renderer::onGammaFactorUpdate(float gammaFactor) {
        this->gammaFactor = gammaFactor;
        applyUpdatedGammaFactor();
    }

    float Renderer::getGammaFactor() const {
        return gammaFactor;
    }

}
