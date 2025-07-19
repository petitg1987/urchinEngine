#pragma once

#include <vector>
#include <memory>

#include <scene/InputDeviceKey.h>
#include <scene/ui/UIRenderer.h>
#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class UiContainer {
        public:
            UiContainer(RenderTarget&, I18nService&);

            void onCameraProjectionUpdate(Camera&);

            bool onKeyPress(InputDeviceKey) const;
            bool onKeyRelease(InputDeviceKey) const;
            bool onChar(char32_t) const;
            bool onMouseMove(double, double, double, double) const;
            bool onScroll(double) const;

            UIRenderer& newUI3dRenderer(const Transform<float>&, const Point2<int>&, const Point2<float>&, float);
            void removeUI3dRenderer(const UIRenderer*);

            void prepareRendering(float, unsigned int&, const Camera&) const;

        private:
            RenderTarget& renderTarget;
            I18nService& i18nService;
            Camera* camera;

            std::vector<std::unique_ptr<UIRenderer>> uis;
    };

}
