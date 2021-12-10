#include <memory>

#include <scene/lights/LightScopeDisplayer.h>

namespace urchin {

    LightScopeDisplayer::LightScopeDisplayer(Scene& scene) :
            scene(scene) {

    }

    LightScopeDisplayer::~LightScopeDisplayer() {
        cleanCurrentDisplay();
    }

    void LightScopeDisplayer::displayLightScope(const LightEntity* lightEntity) {
        cleanCurrentDisplay();

        if (lightEntity) {
            const Light* light = lightEntity->getLight();

            if (const auto* omnidirectionalLight = dynamic_cast<const OmnidirectionalLight*>(light)) {
                auto geometryModel = std::make_unique<SphereModel>(omnidirectionalLight->getSphereScope(), 25);
                geometryModel->setColor(1.0f, 0.0f, 0.0f);
                lightScopeModels.push_back(std::move(geometryModel));
            }

            for (const auto& lightScopeModel : lightScopeModels) {
                scene.getActiveRenderer3d()->getGeometryContainer().addGeometry(lightScopeModel);
            }
        }
    }

    void LightScopeDisplayer::cleanCurrentDisplay() {
        for (const auto& lightScopeModel : lightScopeModels) {
            scene.getActiveRenderer3d()->getGeometryContainer().removeGeometry(*lightScopeModel);
        }
        lightScopeModels.clear();
    }
}
