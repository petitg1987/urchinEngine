#ifndef URCHINENGINE_FPSCAMERA_H
#define URCHINENGINE_FPSCAMERA_H

#include "Camera.h"

namespace urchin
{

    /**
     * A FPS camera:
     * - mouse allow to rotate camera based on mouse movements
     * - directional keys allow to move camera forward/backward/left/right
     */
    class FpsCamera : public Camera
    {
        public:
            FpsCamera(float, float, float);
            ~FpsCamera() override = default;

            void setKeys(unsigned int, unsigned int, unsigned int, unsigned int);
            void setSpeed(float);

            bool onKeyPress(unsigned int) override;
            bool onKeyRelease(unsigned int) override;

            void updateCameraView(float) override;

        private:
            unsigned int keyFront, keyBack, keyLeft, keyRight;
            bool isKeyFrontPressed, isKeyBackPressed, isKeyLeftPressed, isKeyRightPressed;
            float speed;
    };

}

#endif
