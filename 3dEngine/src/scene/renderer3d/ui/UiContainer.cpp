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

    bool UiContainer::onKeyPress(unsigned int key) {
        for (auto& ui : uis) {
            if (!ui->onKeyPress(key)) {
                return false;
            }
        }
        return true;
    }

    bool UiContainer::onKeyRelease(unsigned int key) {
        for (auto& ui : uis) {
            if (!ui->onKeyRelease(key)) {
                return false;
            }
        }
        return true;
    }

    bool UiContainer::onChar(char32_t unicodeCharacter) {
        for (auto& ui : uis) {
            if (!ui->onChar(unicodeCharacter)) {
                return false;
            }
        }
        return true;
    }

    bool UiContainer::onMouseMove(double mouseX, double mouseY) {
        for (auto& ui : uis) {
            if (!ui->onMouseMove(mouseX, mouseY)) {
                return false;
            }
        }
        return true;
    }

    bool UiContainer::onScroll(double offsetY) {
        for (auto& ui : uis) {
            if (!ui->onScroll(offsetY)) {
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

    void UiContainer::prepareRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        for (auto& ui : uis) {
            ui->prepareRendering(dt, renderingOrder, projectionViewMatrix);
        }
    }
}
