#include <scene/renderer3d/camera/FpsCamera.h>

#define DEFAULT_KEY_FRONT 'Z'
#define DEFAULT_KEY_BACK 'S'
#define DEFAULT_KEY_LEFT 'Q'
#define DEFAULT_KEY_RIGHT 'D'
#define DEFAULT_SPEED 10.0

namespace urchin {

    /**
    * @param angle Angle of the field of view (fovy)
    */
    FpsCamera::FpsCamera(float angle, float nearPlane, float farPlane) :
            Camera(angle, nearPlane, farPlane),
            keyFront(DEFAULT_KEY_FRONT),
            keyBack(DEFAULT_KEY_BACK),
            keyLeft(DEFAULT_KEY_LEFT),
            keyRight(DEFAULT_KEY_RIGHT),
            isKeyFrontPressed(false),
            isKeyBackPressed(false),
            isKeyLeftPressed(false),
            isKeyRightPressed(false),
            speed(DEFAULT_SPEED) {
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

    void FpsCamera::updateCameraView(float dt) {
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
