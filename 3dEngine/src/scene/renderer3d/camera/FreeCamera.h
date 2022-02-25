#pragma once

#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    /**
     * A free camera:
     * - mouse right click released: directional keys allow to move camera forward/backward and rotate camera
     * - mouse right click pressed: camera rotate based on mouse movements and directional keys allow to move camera forward/backward/left/right
     */
    class FreeCamera : public Camera {
        public:
            FreeCamera(float, float, float);
            ~FreeCamera() override = default;

            void setKeys(unsigned int, unsigned int, unsigned int, unsigned int);
            void setSpeed(float, float);

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
            float rotateSpeed;
    };

}
