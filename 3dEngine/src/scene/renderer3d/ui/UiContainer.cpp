#include <scene/renderer3d/ui/UiContainer.h>

namespace urchin {

    UiContainer::UiContainer(RenderTarget& renderTarget, I18nService& i18nService) :
            renderTarget(renderTarget),
            i18nService(i18nService) {

    }

    void UiContainer::onResize(unsigned int width, unsigned int height) {
        for(auto& ui : uis) {
            ui->onResize(width, height);
        }
    }

    void UiContainer::onCameraProjectionUpdate(const Camera& camera) {
        this->projectionMatrix = camera.getProjectionMatrix();

        for(auto& ui : uis) {
            ui->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    UIRenderer& UiContainer::newUI3dRenderer(const Transform<float>& transform) {
        auto uiRenderer = std::make_unique<UIRenderer>(renderTarget, i18nService);
        uiRenderer->initializeFor3dUi(projectionMatrix, transform);
        uis.push_back(std::move(uiRenderer));
        return *uis.back();
    }

    void UiContainer::prepareRendering(float dt, unsigned int& screenRenderingOrder, const Matrix4<float>& viewMatrix) {
        for(auto& ui : uis) {
            ui->prepareRendering(dt, screenRenderingOrder, viewMatrix);
        }
    }
}
