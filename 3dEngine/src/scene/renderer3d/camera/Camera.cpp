#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    Camera::Camera(float horizontalFovAngle, float nearPlane, float farPlane) :
            MOUSE_SENSITIVITY_FACTOR(ConfigService::instance().getFloatValue("camera.mouseSensitivityFactor")),
            mView(Matrix4<float>()),
            mProjection(Matrix4<float>()),
            position(Point3(0.0f, 0.0f, 0.0f)),
            view(Vector3(0.0f, 0.0f, -1.0f)),
            up(Vector3(0.0f, 1.0f, 0.0f)),
            horizontalFovAngle(horizontalFovAngle),
            nearPlane(nearPlane),
            farPlane(farPlane),
            maxRotationX(0.995f),
            distance(0.0f),
            useMouse(false),
            mouseSensitivityPercentage(1.0f),
            invertYAxis(false),
            sceneWidth(0),
            sceneHeight(0) {
        float verticalFovAngle = computeVerticalFovAngle();
        baseFrustum = Frustum(verticalFovAngle, 1.0f, nearPlane, farPlane);
        frustum = Frustum(verticalFovAngle, 1.0f, nearPlane, farPlane);
    }

    void Camera::initialize(unsigned int sceneWidth, unsigned int sceneHeight) {
        initializeOrUpdate(sceneWidth, sceneHeight);
    }

    void Camera::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        initializeOrUpdate(sceneWidth, sceneHeight);
    }

    unsigned int Camera::getSceneWidth() const {
        return sceneWidth;
    }

    unsigned int Camera::getSceneHeight() const {
        return sceneHeight;
    }

    void Camera::initializeOrUpdate(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        //projection matrix
        float verticalFovAngle = computeVerticalFovAngle();
        float fov = computeFov(verticalFovAngle);
        float ratio = (float)sceneWidth / (float)sceneHeight;
        mProjection.setValues(
                fov / ratio, 0.0f, 0.0f, 0.0f,
                0.0f, -fov, 0.0f, 0.0f,
                0.0f, 0.0f, farPlane / (nearPlane - farPlane), (farPlane * nearPlane) / (nearPlane - farPlane),
                0.0f, 0.0f, -1.0f, 0.0f);

        baseFrustum.buildFrustum(verticalFovAngle, ratio, nearPlane, farPlane);

        updateComponents();
        notifyObservers(this, PROJECTION_UPDATED);
    }

    float Camera::computeVerticalFovAngle() const {
        if (sceneHeight != 0 && sceneWidth != 0) {
            float horizontalFovRadian = AngleConverter<float>::toRadian(horizontalFovAngle);
            float verticalFovRadian = 2.0f * std::atan(std::tan(horizontalFovRadian / 2.0f) * (float)sceneHeight / (float)sceneWidth);
            return AngleConverter<float>::toDegree(verticalFovRadian);
        }
        return 1.0f;
    }

    void Camera::useMouseToMoveCamera(bool use) {
        useMouse = use;
    }

    bool Camera::isUseMouseToMoveCamera() const {
        return useMouse;
    }

    void Camera::setMouseSensitivityPercentage(float mouseSensitivityPercentage) {
        this->mouseSensitivityPercentage = mouseSensitivityPercentage;
    }

    void Camera::setInvertYAxis(bool invertYAxis) {
        this->invertYAxis = invertYAxis;
    }

    /**
    * @param distance Distance between the camera and the rotation point (0 : first person camera | >0 : third person camera)
    */
    void Camera::setDistance(float distance) {
        this->distance = distance;
    }

    bool Camera::isFirstPersonCamera() const {
        return MathFunction::isZero(distance);
    }

    void Camera::setMaxRotationX(float maxRotationX) {
        this->maxRotationX = maxRotationX;
    }

    void Camera::applyJitter(float x, float y) {
        mProjection.a13 = x;
        mProjection.a23 = y;

        updateComponents();
    }

    const Matrix4<float>& Camera::getViewMatrix() const {
        return mView;
    }

    const Matrix4<float>& Camera::getProjectionMatrix() const {
        return mProjection;
    }

    const Matrix4<float>& Camera::getProjectionViewMatrix() const {
        return mProjectionView;
    }

    const Matrix4<float>& Camera::getProjectionViewInverseMatrix() const {
        return mProjectionViewInverse;
    }

    const Matrix4<float>& Camera::getProjectionInverseMatrix() const {
        return mProjectionInverse;
    }

    const Quaternion<float>& Camera::getOrientation() const {
        return orientation;
    }

    const Point3<float>& Camera::getPosition() const {
        return position;
    }

    const Vector3<float>& Camera::getView() const {
        return view;
    }

    const Vector3<float>& Camera::getUp() const {
        return up;
    }

    float Camera::getHorizontalFovAngle() const {
        return horizontalFovAngle;
    }

    void Camera::updateHorizontalFovAngle(float horizontalFovAngle) {
        if (!MathFunction::isEqual(horizontalFovAngle, this->horizontalFovAngle, 0.001f)) {
            this->horizontalFovAngle = horizontalFovAngle;
            initializeOrUpdate(sceneWidth, sceneHeight);
        }
    }

    float Camera::computeFov() const {
        float verticalFovAngle = computeVerticalFovAngle();
        return computeFov(verticalFovAngle);
    }

    float Camera::computeFov(float verticalFovAngle) const {
        return 1.0f / std::tan((verticalFovAngle * MathValue::PI_FLOAT) / 360.0f);
    }

    float Camera::getNearPlane() const {
        return nearPlane;
    }

    float Camera::getFarPlane() const {
        return farPlane;
    }

    const Frustum<float>& Camera::getFrustum() const {
        return frustum;
    }

    void Camera::moveTo(const Point3<float>& position) {
        if (this->position != position) {
            this->position = position;

            updateComponents();
            notifyObservers(this, POSITION_UPDATED);
        }
    }

    void Camera::moveOnLocalXAxis(float distance) {
        Vector3<float> localXAxis = up.crossProduct(view).normalize();
        position = position.translate(localXAxis * distance);

        updateComponents();
        notifyObservers(this, POSITION_UPDATED);
    }

    void Camera::moveOnLocalZAxis(float distance) {
        Vector3<float> localZAxis = view;
        position = position.translate(localZAxis * distance);

        updateComponents();
        notifyObservers(this, POSITION_UPDATED);
    }

    void Camera::rotate(const Quaternion<float>& rotationDelta) {
        Point3<float> pivot = position;
        if (!isFirstPersonCamera()) {
            pivot = position.translate(view * distance);
        }

        //moves view point
        Point3<float> viewPoint = pivot.translate(rotationDelta.rotateVector(view));
        view = position.vector(viewPoint).normalize();

        //moves up vector
        up = rotationDelta.rotateVector(up);
        up = up.normalize(); //normalize is required because due to float imprecision, the vector becomes less and less normalized

        //moves position point
        if (!isFirstPersonCamera()) {
            Vector3<float> axis = pivot.vector(position);
            position = pivot.translate(rotationDelta.rotateVector(axis));

            updateComponents();
            notifyObservers(this, POSITION_UPDATED);
        } else {
            updateComponents();
        }
    }

    void Camera::updateViewUp(const Vector3<float>& view, const Vector3<float>& up) {
        this->view = view;
        this->up = up;
        updateComponents();
    }

    bool Camera::onKeyPress(InputDeviceKey) {
        //do nothing
        return true;
    }

    bool Camera::onKeyRelease(InputDeviceKey) {
        //do nothing
        return true;
    }

    bool Camera::onMouseMove(double deltaMouseX, double deltaMouseY) {
        if (useMouse) {
            if (deltaMouseX == 0.0 && deltaMouseY == 0.0) {
                return false;
            }

            Vector2<float> mouseDirection;
            mouseDirection.X = (float)(-deltaMouseX * (double)MOUSE_SENSITIVITY_FACTOR * (double)mouseSensitivityPercentage);
            mouseDirection.Y = (float)(-deltaMouseY * (double)MOUSE_SENSITIVITY_FACTOR * (double)mouseSensitivityPercentage);
            if (invertYAxis) {
                mouseDirection.Y = - mouseDirection.Y;
            }

            //do not rotate up/down more than "maxRotationX" percent
            float currentRotationX = view.Y + mouseDirection.Y;
            if (currentRotationX > 0.0f && currentRotationX > maxRotationX) {
                mouseDirection.Y -= (currentRotationX - maxRotationX);
            } else if (currentRotationX < 0.0f && currentRotationX < -maxRotationX) {
                mouseDirection.Y -= (currentRotationX + maxRotationX);
            }

            //rotate around the Y and X axis
            rotate(Quaternion<float>::fromAxisAngle(up.crossProduct(view), -mouseDirection.Y));
            rotate(Quaternion<float>::rotationY(mouseDirection.X));

            updateComponents();

            return false;
        }
        return true;
    }

    void Camera::updateComponents() {
        const Vector3<float> viewUp = view.crossProduct(up).normalize();
        mView.setValues(
                viewUp[0],  viewUp[1],  viewUp[2],  viewUp[0] * -position.X + viewUp[1] * -position.Y + viewUp[2] * -position.Z,
                up[0],      up[1],      up[2],      up[0] * -position.X + up[1] * -position.Y + up[2] * -position.Z,
                -view[0],   -view[1],   -view[2],   -view[0] * -position.X + -view[1] * -position.Y + -view[2] * -position.Z,
                0.0f,       0.0f,       0.0f,       1.0f);
        frustum = baseFrustum * mView.inverse();

        //pre-computed values
        orientation = Quaternion<float>::fromRotationMatrix(mView.toMatrix3()).conjugate();
        mProjectionView = mProjection * mView;
        mProjectionViewInverse = mProjectionView.inverse();
        mProjectionInverse = mProjection.inverse();
    }

    void Camera::moveForward(float) {
        //do nothing (can be overridden)
    }

    void Camera::moveBackward(float) {
        //do nothing (can be overridden)
    }

    void Camera::moveLeft(float) {
        //do nothing (can be overridden)
    }

    void Camera::moveRight(float) {
        //do nothing (can be overridden)
    }

}
