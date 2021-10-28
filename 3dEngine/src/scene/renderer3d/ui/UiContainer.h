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

            UIRenderer& newUI3dRenderer(const Transform<float>&);

            void prepareRendering(float, unsigned int&, const Matrix4<float>&);

        private:
            RenderTarget& renderTarget;
            I18nService& i18nService;

            Matrix4<float> projectionMatrix;

            std::vector<std::unique_ptr<UIRenderer>> uis;
    };

}
