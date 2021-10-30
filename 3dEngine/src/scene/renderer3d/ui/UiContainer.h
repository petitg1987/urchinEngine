#pragma once

#include <vector>
#include <memory>

#include <scene/ui/UIRenderer.h>
#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class UiContainer {
        public:
            UiContainer(RenderTarget&, I18nService&);

            void onCameraProjectionUpdate(const Camera&);

            bool onMouseMove(double, double);

            UIRenderer& newUI3dRenderer(const Transform<float>&, const Point2<unsigned int>&, const Point2<float>&, float);

            void prepareRendering(float, unsigned int&, const Matrix4<float>&);

        private:
            RenderTarget& renderTarget;
            I18nService& i18nService;
            const Camera* camera;

            std::vector<std::unique_ptr<UIRenderer>> uis;
    };

}
