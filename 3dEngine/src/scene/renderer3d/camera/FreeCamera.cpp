#include <UrchinCommon.h>

#include "scene/renderer3d/camera/FreeCamera.h"

namespace urchin {

    FreeCamera::FreeCamera(float horizontalFovAngle, float nearPlane, float farPlane) :
            Camera(horizontalFovAngle, nearPlane, farPlane),
            speed(10.0f),
            rotateSpeed(2.0f) {

    }

    void FreeCamera::setSpeed(float speed, float rotateSpeed) {
        this->speed = speed;
        this->rotateSpeed = rotateSpeed;
    }

    bool FreeCamera::onKeyPress(InputKey key) {
        if (key == InputKey::RMB) {
            useMouseToMoveCamera(true);
            return false;
        }
        return true;
    }

    bool FreeCamera::onKeyRelease(InputKey key) {
        if (key == InputKey::RMB) {
            useMouseToMoveCamera(false);
            return false;
        }
        return true;
    }

    void FreeCamera::moveForward(float dt) {
        moveOnLocalZAxis(dt * speed);
    }

    void FreeCamera::moveBackward(float dt) {
        moveOnLocalZAxis(-dt * speed);
    }

    void FreeCamera::moveLeft(float dt) {
        if (isUseMouseToMoveCamera()) {
            moveOnLocalXAxis(dt * speed);
        } else {
            rotate(Quaternion<float>::rotationY(dt * rotateSpeed));
        }
    }

    void FreeCamera::moveRight(float dt) {
        if (isUseMouseToMoveCamera()) {
            moveOnLocalXAxis(-dt * speed);
        } else {
            rotate(Quaternion<float>::rotationY(-dt * rotateSpeed));
        }
    }

}
