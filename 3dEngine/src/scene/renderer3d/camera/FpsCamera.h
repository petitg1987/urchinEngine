#pragma once

#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    /**
     * A FPS camera:
     * - mouse allow to rotate camera based on mouse movements
     * - directional keys allow to move camera forward/backward/left/right
     */
    class FpsCamera : public Camera {
        public:
            FpsCamera(float, float, float);
            ~FpsCamera() override = default;

            void setKeys(unsigned int, unsigned int, unsigned int, unsigned int);
            void setSpeed(float);

            bool onKeyPress(unsigned int) override;
            bool onKeyRelease(unsigned int) override;

            void updateCameraView(float) override;

        private:
            unsigned int keyFront;
            unsigned int keyBack;
            unsigned int keyLeft;
            unsigned int keyRight;
            bool isKeyFrontPressed;
            bool isKeyBackPressed;
            bool isKeyLeftPressed;
            bool isKeyRightPressed;
            float speed;
    };

}
