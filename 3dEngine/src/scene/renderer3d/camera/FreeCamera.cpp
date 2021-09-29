#include <UrchinCommon.h>

#include <scene/renderer3d/camera/FreeCamera.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    FreeCamera::FreeCamera(float horizontalFovAngle, float nearPlane, float farPlane) :
            Camera(horizontalFovAngle, nearPlane, farPlane),
            keyFront('Z'),
            keyBack('S'),
            keyLeft('Q'),
            keyRight('D'),
            isKeyFrontPressed(false),
            isKeyBackPressed(false),
            isKeyLeftPressed(false),
            isKeyRightPressed(false),
            speed(10.0f),
            rotateSpeed(2.0f) {

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
        if (key == (int)InputDeviceKey::MOUSE_RIGHT) {
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
        if (key == (int)InputDeviceKey::MOUSE_RIGHT) {
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
                rotate(Quaternion<float>::rotationY(dt * rotateSpeed));
            }
        }

        if (isKeyRightPressed) {
            if (isUseMouseToMoveCamera()) {
                moveOnLocalXAxis(-dt * speed);
            } else {
                rotate(Quaternion<float>::rotationY(-dt * rotateSpeed));
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
