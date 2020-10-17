#include <memory>

#include "LightScopeDisplayer.h"

namespace urchin {

    LightScopeDisplayer::LightScopeDisplayer(SceneManager *sceneManager) :
        sceneManager(sceneManager) {

    }

    LightScopeDisplayer::~LightScopeDisplayer() {
        cleanCurrentDisplay();
    }

    void LightScopeDisplayer::displayLightScopeFor(const SceneLight *sceneLight) {
        cleanCurrentDisplay();

        if (sceneLight) {
            const Light *light = sceneLight->getLight();

            if (const auto *omnidirectionLight = dynamic_cast<const OmnidirectionalLight *>(light)) {
                auto *geometryModel = new SphereModel(omnidirectionLight->getSphereScope(), 25);
                lightScopeModels.push_back(geometryModel);
            }

            for (auto &lightScopeModel : lightScopeModels) {
                lightScopeModel->setColor(1.0, 0.0, 0.0);
                sceneManager->getActiveRenderer3d()->getGeometryManager()->addGeometry(lightScopeModel);
            }
        }
    }

    void LightScopeDisplayer::cleanCurrentDisplay() {
        for (auto &lightScopeModel : lightScopeModels) {
            sceneManager->getActiveRenderer3d()->getGeometryManager()->removeGeometry(lightScopeModel);
            delete lightScopeModel;
        }

        lightScopeModels.clear();
    }
}
