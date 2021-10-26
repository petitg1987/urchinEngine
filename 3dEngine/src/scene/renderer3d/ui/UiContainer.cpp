#include <scene/renderer3d/ui/UiContainer.h>

namespace urchin {

    void UiContainer::onResize(unsigned int width, unsigned int height) {
        for(auto& ui : uis) {
            ui->onResize(width, height);
        }
    }

    void UiContainer::onCameraProjectionUpdate(const Camera&) {
        //TODO ...
    }

    void UiContainer::addUi(std::unique_ptr<UIRenderer> ui) {
        uis.push_back(std::move(ui));
    }

    void UiContainer::prepareRendering(float dt, unsigned int& screenRenderingOrder) {
        for(auto& ui : uis) {
            ui->prepareRendering(dt, screenRenderingOrder);
        }
    }
}
