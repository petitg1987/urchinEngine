#include <scene/renderer3d/ui/UiContainer.h>

namespace urchin {

    void UiContainer::onResize(unsigned int width, unsigned int height) {
        for(auto& ui : uis) {
            ui->onResize(width, height);
        }
    }

    void UiContainer::onCameraProjectionUpdate(const Camera& camera) {
        for(auto& ui : uis) {
            ui->onCameraProjectionUpdate(camera.getProjectionMatrix());
        }
    }

    void UiContainer::addUi(std::unique_ptr<UIRenderer> ui) {
        uis.push_back(std::move(ui));
    }

    void UiContainer::prepareRendering(float dt, unsigned int& screenRenderingOrder, const Matrix4<float>& viewMatrix) {
        for(auto& ui : uis) {
            ui->prepareRendering(dt, screenRenderingOrder, viewMatrix);
        }
    }
}
