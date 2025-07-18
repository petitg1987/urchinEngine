#include <memory>

#include <scene/object/light/LightScopeDisplayer.h>

namespace urchin {

    LightScopeDisplayer::LightScopeDisplayer(Scene& scene) :
            scene(scene) {

    }

    LightScopeDisplayer::~LightScopeDisplayer() {
        cleanCurrentDisplay();
    }

    void LightScopeDisplayer::displayLightScopes(const std::vector<const ObjectEntity*>& objectEntities) {
        cleanCurrentDisplay();

        for (const ObjectEntity* objectEntity : objectEntities) {
            if (!objectEntity->getLight()) {
                continue;
            }
            const Light* light = objectEntity->getLight();

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

                auto frustumModel = std::make_unique<FrustumModel>(spotLight->getFrustumScope());
                frustumModel->setColor(Vector3(1.0f, 0.5f, 0.5f));
                frustumModel->setPolygonMode(PolygonMode::WIREFRAME);
                lightScopeModels.push_back(std::move(frustumModel));
            }
        }

        for (const auto& lightScopeModel : lightScopeModels) {
            scene.getActiveRenderer3d()->getGeometryContainer().addGeometry(lightScopeModel);
        }
    }

    void LightScopeDisplayer::cleanCurrentDisplay() {
        for (const auto& lightScopeModel : lightScopeModels) {
            scene.getActiveRenderer3d()->getGeometryContainer().removeGeometry(*lightScopeModel);
        }
        lightScopeModels.clear();
    }
}
