#include <scene/renderer3d/camera/FpsCamera.h>

namespace urchin {

    FpsCamera::FpsCamera(float horizontalFovAngle, float nearPlane, float farPlane) :
            Camera(horizontalFovAngle, nearPlane, farPlane),
            keyFront('Z'),
            keyBack('S'),
            keyLeft('Q'),
            keyRight('D'),
            isKeyFrontPressed(false),
            isKeyBackPressed(false),
            isKeyLeftPressed(false),
            isKeyRightPressed(false),
            speed(10.0f) {
        useMouseToMoveCamera(true);
    }

    /**
    * Specifies the keys to move the camera
    */
    void FpsCamera::setKeys(unsigned int keyFront, unsigned int keyBack, unsigned int keyLeft, unsigned int keyRight) {
        this->keyFront = keyFront;
        this->keyBack = keyBack;
        this->keyLeft = keyLeft;
        this->keyRight = keyRight;
    }

    void FpsCamera::setSpeed(float speed) {
        this->speed = speed;
    }

    bool FpsCamera::onKeyPress(unsigned int key) {
        if (key == keyFront) {
            isKeyFrontPressed = true;
            return false;
        } else if (key == keyBack) {
            isKeyBackPressed = true;
            return false;
        } else if (key == keyLeft) {
            isKeyLeftPressed = true;
            return false;
        } else if (key == keyRight) {
            isKeyRightPressed = true;
            return false;
        }
        return true;
    }

    bool FpsCamera::onKeyRelease(unsigned int key) {
        if (key == keyFront) {
            isKeyFrontPressed = false;
            return false;
        } else if (key == keyBack) {
            isKeyBackPressed = false;
            return false;
        } else if (key == keyLeft) {
            isKeyLeftPressed = false;
            return false;
        } else if (key == keyRight) {
            isKeyRightPressed = false;
            return false;
        }
        return true;
    }

    void FpsCamera::refreshCameraView(float dt) {
        if (isKeyLeftPressed) {
            moveOnLocalXAxis(dt * speed);
        }

        if (isKeyRightPressed) {
            moveOnLocalXAxis(-dt * speed);
        }

        if (isKeyFrontPressed) {
            moveOnLocalZAxis(dt * speed);
        }

        if (isKeyBackPressed) {
            moveOnLocalZAxis(-dt * speed);
        }
    }

}
