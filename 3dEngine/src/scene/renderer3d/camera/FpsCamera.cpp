#include "scene/renderer3d/camera/FpsCamera.h"

namespace urchin {

    FpsCamera::FpsCamera(float horizontalFovAngle, float nearPlane, float farPlane) :
            Camera(horizontalFovAngle, nearPlane, farPlane),
            speed(10.0f) {
        useMouseToMoveCamera(true);
    }

    void FpsCamera::setSpeed(float speed) {
        this->speed = speed;
    }

    void FpsCamera::moveForward(float dt) {
        moveOnLocalZAxis(dt * speed);
    }

    void FpsCamera::moveBackward(float dt) {
        moveOnLocalZAxis(-dt * speed);
    }

    void FpsCamera::moveLeft(float dt) {
        moveOnLocalXAxis(dt * speed);
    }

    void FpsCamera::moveRight(float dt) {
        moveOnLocalXAxis(-dt * speed);
    }

}
