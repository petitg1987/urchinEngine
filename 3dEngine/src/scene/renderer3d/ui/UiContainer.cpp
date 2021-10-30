#include <scene/renderer3d/ui/UiContainer.h>

namespace urchin {

    UiContainer::UiContainer(RenderTarget& renderTarget, I18nService& i18nService) :
            renderTarget(renderTarget),
            i18nService(i18nService),
            camera(nullptr) {

    }

    void UiContainer::onCameraProjectionUpdate(const Camera& camera) {
        this->camera = &camera;

        for (auto& ui : uis) {
            ui->onCameraProjectionUpdate(camera);
        }
    }

    bool UiContainer::onMouseMove(double mouseX, double mouseY) {
        for (auto& ui : uis) {
            if (!ui->onMouseMove(mouseX, mouseY)) {
                return false;
            }
        }
        return true;
    }

    UIRenderer& UiContainer::newUI3dRenderer(const Transform<float>& transform, const Point2<unsigned int>& sceneResolution, const Point2<float>& uiSize, float ambient) {
        auto uiRenderer = std::make_unique<UIRenderer>(renderTarget, i18nService);
        uiRenderer->setupUi3d(camera, transform, sceneResolution, uiSize, ambient);
        uis.push_back(std::move(uiRenderer));
        return *uis.back();
    }

    void UiContainer::prepareRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& viewMatrix) {
        for (auto& ui : uis) {
            ui->prepareRendering(dt, renderingOrder, viewMatrix);
        }
    }
}
