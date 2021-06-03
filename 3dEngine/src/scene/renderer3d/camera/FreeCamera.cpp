#include <UrchinCommon.h>

#include "FreeCamera.h"
#include <scene/InputDeviceKey.h>

#define DEFAULT_KEY_FRONT 'Z'
#define DEFAULT_KEY_BACK 'S'
#define DEFAULT_KEY_LEFT 'Q'
#define DEFAULT_KEY_RIGHT 'D'
#define DEFAULT_SPEED 10.0
#define DEFAULT_ROTATE_SPEED 2.0

namespace urchin {

    /**
    * @param angle Angle of the field of view (fovy)
    */
    FreeCamera::FreeCamera(float angle, float nearPlane, float farPlane) :
            Camera(angle, nearPlane, farPlane),
            keyFront(DEFAULT_KEY_FRONT),
            keyBack(DEFAULT_KEY_BACK),
            keyLeft(DEFAULT_KEY_LEFT),
            keyRight(DEFAULT_KEY_RIGHT),
            isKeyFrontPressed(false),
            isKeyBackPressed(false),
            isKeyLeftPressed(false),
            isKeyRightPressed(false),
            speed(DEFAULT_SPEED),
            rotateSpeed(DEFAULT_ROTATE_SPEED) {

    }

    /**
    * Specifies the keys to move the camera
    */
    void FreeCamera::setKeys(unsigned int keyFront, unsigned int keyBack, unsigned int keyLeft, unsigned int keyRight) {
        this->keyFront = keyFront;
        this->keyBack = keyBack;
        this->keyLeft = keyLeft;
        this->keyRight = keyRight;
    }

    void FreeCamera::setSpeed(float speed, float rotateSpeed) {
        this->speed = speed;
        this->rotateSpeed = rotateSpeed;
    }

    bool FreeCamera::onKeyPress(unsigned int key) {
        if (key == InputDeviceKey::MOUSE_RIGHT) {
            useMouseToMoveCamera(true);
            return false;
        } else if (key == keyFront) {
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

    bool FreeCamera::onKeyRelease(unsigned int key) {
        if (key == InputDeviceKey::MOUSE_RIGHT) {
            useMouseToMoveCamera(false);
            return false;
        } else if (key == keyFront) {
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

    void FreeCamera::updateCameraView(float dt) {
        if (isKeyLeftPressed) {
            if (isUseMouseToMoveCamera()) {
                moveOnLocalXAxis(dt * speed);
            } else {
                rotate(Quaternion<float>(Vector3<float>(0.0, 1.0f, 0.0), dt * rotateSpeed));
            }
        }

        if (isKeyRightPressed) {
            if (isUseMouseToMoveCamera()) {
                moveOnLocalXAxis(-dt * speed);
            } else {
                rotate(Quaternion<float>(Vector3<float>(0.0, 1.0f, 0.0), -dt * rotateSpeed));
            }
        }

        if (isKeyFrontPressed) {
            moveOnLocalZAxis(dt * speed);
        }

        if (isKeyBackPressed) {
            moveOnLocalZAxis(-dt * speed);
        }
    }

}
