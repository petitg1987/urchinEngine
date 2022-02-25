#include <scene/renderer3d/ui/UiContainer.h>

namespace urchin {

    UiContainer::UiContainer(RenderTarget& renderTarget, I18nService& i18nService) :
            renderTarget(renderTarget),
            i18nService(i18nService),
            camera(nullptr) {

    }

    void UiContainer::onCameraProjectionUpdate(Camera& camera) {
        this->camera = &camera;

        for (const auto& ui : uis) {
            ui->onCameraProjectionUpdate(camera);
        }
    }

    bool UiContainer::onKeyPress(unsigned int key) const {
        for (const auto& ui : uis) {
            if (!ui->onKeyPress(key)) {
                return false;
            }
        }
        return true;
    }

    bool UiContainer::onKeyRelease(unsigned int key) const {
        for (const auto& ui : uis) {
            if (!ui->onKeyRelease(key)) {
                return false;
            }
        }
        return true;
    }

    bool UiContainer::onChar(char32_t unicodeCharacter) const {
        for (const auto& ui : uis) {
            if (!ui->onChar(unicodeCharacter)) {
                return false;
            }
        }
        return true;
    }

    bool UiContainer::onMouseMove(double mouseX, double mouseY) const {
        for (const auto& ui : uis) {
            if (!ui->onMouseMove(mouseX, mouseY)) {
                return false;
            }
        }
        return true;
    }

    bool UiContainer::onScroll(double offsetY) const {
        for (const auto& ui : uis) {
            if (!ui->onScroll(offsetY)) {
                return false;
            }
        }
        return true;
    }

    UIRenderer& UiContainer::newUI3dRenderer(const Transform<float>& transform, const Point2<int>& sceneResolution, const Point2<float>& uiSize, float ambient) {
        auto uiRenderer = std::make_unique<UIRenderer>(renderTarget, i18nService);
        uiRenderer->setupUi3d(camera, transform, sceneResolution, uiSize, ambient);
        uis.push_back(std::move(uiRenderer));
        return *uis.back();
    }

    void UiContainer::removeUI3dRenderer(const UIRenderer* ui3dRenderer) {
        std::erase_if(uis, [&](auto& p){ return ui3dRenderer == p.get(); });
    }

    void UiContainer::prepareRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) const {
        for (const auto& ui : uis) {
            ui->prepareRendering(dt, renderingOrder, projectionViewMatrix);
        }
    }
}
