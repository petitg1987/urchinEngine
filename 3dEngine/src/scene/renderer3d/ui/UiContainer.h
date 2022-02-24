#pragma once

#include <vector>
#include <memory>

#include <scene/ui/UIRenderer.h>
#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class UiContainer {
        public:
            UiContainer(RenderTarget&, I18nService&);

            void onCameraProjectionUpdate(Camera&);

            bool onKeyPress(unsigned int);
            bool onKeyRelease(unsigned int);
            bool onChar(char32_t);
            bool onMouseMove(double, double);
            bool onScroll(double);

            UIRenderer& newUI3dRenderer(const Transform<float>&, const Point2<int>&, const Point2<float>&, float);
            void removeUI3dRenderer(const UIRenderer*);

            void prepareRendering(float, unsigned int&, const Matrix4<float>&);

        private:
            RenderTarget& renderTarget;
            I18nService& i18nService;
            Camera* camera;

            std::vector<std::unique_ptr<UIRenderer>> uis;
    };

}
