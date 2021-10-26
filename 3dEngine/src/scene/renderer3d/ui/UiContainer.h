#pragma once

#include <vector>
#include <memory>

#include <scene/ui/UIRenderer.h>
#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class UiContainer {
        public:
            void onResize(unsigned int, unsigned int);
            void onCameraProjectionUpdate(const Camera&);

            void addUi(std::unique_ptr<UIRenderer>);

            void prepareRendering(float, unsigned int&);

        private:
            std::vector<std::unique_ptr<UIRenderer>> uis;
    };

}
