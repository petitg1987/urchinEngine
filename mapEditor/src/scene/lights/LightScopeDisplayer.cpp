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
                auto sphereModel = std::make_unique<SphereModel>(omnidirectionalLight->getSphereScope(), 25);
                sphereModel->setColor(Vector3(1.0f, 0.0f, 0.0f));
                sphereModel->setPolygonMode(PolygonMode::WIREFRAME);
                lightScopeModels.push_back(std::move(sphereModel));

                auto boundingBoxModel = std::make_unique<AABBoxModel>(omnidirectionalLight->getAABBox());
                boundingBoxModel->setColor(Vector3(1.0f, 0.5f, 0.0f));
                boundingBoxModel->setPolygonMode(PolygonMode::WIREFRAME);
                lightScopeModels.push_back(std::move(boundingBoxModel));
            } else if (const auto* spotLight = dynamic_cast<const SpotLight*>(light)) {
                auto coneModel = std::make_unique<ConeModel>(spotLight->getConeScope(), 25);
                coneModel->setColor(Vector3(1.0f, 0.0f, 0.0f));
                coneModel->setPolygonMode(PolygonMode::WIREFRAME);
                lightScopeModels.push_back(std::move(coneModel));

                auto boundingBoxModel = std::make_unique<AABBoxModel>(spotLight->getAABBox());
                boundingBoxModel->setColor(Vector3(1.0f, 0.5f, 0.0f));
                boundingBoxModel->setPolygonMode(PolygonMode::WIREFRAME);
                lightScopeModels.push_back(std::move(boundingBoxModel));
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
