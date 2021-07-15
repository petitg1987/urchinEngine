#include <limits>

#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    /**
    * @param angle Angle of the field of view (fovy)
    */
    Camera::Camera(float angle, float nearPlane, float farPlane) :
            MOUSE_SENSITIVITY_FACTOR(ConfigService::instance().getFloatValue("camera.mouseSensitivityFactor")),
            mView(Matrix4<float>()),
            mProjection(Matrix4<float>()),
            angle(angle),
            nearPlane(nearPlane),
            farPlane(farPlane),
            baseFrustum(Frustum<float>(angle, 1.0f, nearPlane, farPlane)),
            frustum(Frustum<float>(angle, 1.0f, nearPlane, farPlane)),
            position(Point3<float>(0.0f, 0.0f, 0.0f)),
            view(Vector3<float>(0.0f, 0.0f, -1.0f)),
            up(Vector3<float>(0.0f, 1.0f, 0.0f)),
            maxRotationX(0.995f),
            distance(0.0f),
            bUseMouse(false),
            mouseSensitivityPercentage(1.0f),
            invertYAxis(false),
            sceneWidth(0),
            sceneHeight(0),
            previousMouseX(0.0),
            previousMouseY(0.0) {
        resetPreviousMousePosition();
    }

    void Camera::initialize(unsigned int sceneWidth, unsigned int sceneHeight) {
        initializeOrUpdate(sceneWidth, sceneHeight);
    }

    void Camera::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        initializeOrUpdate(sceneWidth, sceneHeight);
    }

    void Camera::initializeOrUpdate(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        //projection matrix
        float fov = 1.0f / std::tan((angle * MathValue::PI_FLOAT) / 360.0f);
        float ratio = (float)sceneWidth / (float)sceneHeight;
        mProjection.setValues(
                fov/ratio, 0.0f, 0.0f, 0.0f,
                0.0f, -fov, 0.0f, 0.0f,
                0.0f, 0.0f, 0.5f * ((farPlane + nearPlane) / (nearPlane - farPlane)) - 0.5f, (farPlane * nearPlane) / (nearPlane - farPlane),
                0.0f, 0.0f, -1.0f, 0.0f);

        //frustum
        baseFrustum.buildFrustum(angle, ratio, nearPlane, farPlane);
        frustum = baseFrustum * mView.inverse();
    }

    void Camera::resetPreviousMousePosition(double previousMouseX, double previousMouseY) {
        if (bUseMouse) {
            this->previousMouseX = previousMouseX;
            this->previousMouseY = previousMouseY;
        }
    }

    void Camera::useMouseToMoveCamera(bool use) {
        bUseMouse = use;

        if (bUseMouse) {
            resetPreviousMousePosition();
        }
    }

    bool Camera::isUseMouseToMoveCamera() const {
        return bUseMouse;
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

    void Camera::setMaxRotationX(float maxRotationX) {
        this->maxRotationX = maxRotationX;
    }

    const Matrix4<float>& Camera::getViewMatrix() const {
        return mView;
    }

    const Matrix4<float>& Camera::getProjectionMatrix() const {
        return mProjection;
    }

    const Frustum<float>& Camera::getFrustum() const {
        return frustum;
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

    unsigned int Camera::getSceneWidth() const {
        return sceneWidth;
    }

    unsigned int Camera::getSceneHeight() const {
        return sceneHeight;
    }

    float Camera::getAngle() const {
        return angle;
    }

    float Camera::getNearPlane() const {
        return nearPlane;
    }

    float Camera::getFarPlane() const {
        return farPlane;
    }

    void Camera::moveTo(const Point3<float>& position) {
        this->position = position;

        updateViewMatrix();
    }

    void Camera::moveOnLocalXAxis(float distance) {
        Vector3<float> localXAxis = up.crossProduct(view).normalize();
        position = position.translate(localXAxis * distance);

        updateViewMatrix();
    }

    void Camera::moveOnLocalZAxis(float distance) {
        Vector3<float> localZAxis = view;
        position = position.translate(localZAxis * distance);

        updateViewMatrix();
    }

    void Camera::lookAt(const Vector3<float>& view) {
        this->view = view.normalize();

        updateViewMatrix();
    }

    void Camera::rotate(const Quaternion<float>& quatRotation) {
        Point3<float> pivot;
        if (std::fabs(distance) > std::numeric_limits<float>::epsilon()) {
            pivot = position.translate(view * distance);
        } else {
            pivot = position;
        }

        //moves view point
        Quaternion<float> quatView(view.X, view.Y, view.Z, 0.0f);
        const Quaternion<float>& resultView = (quatRotation * quatView.normalize()) * quatRotation.conjugate();
        Point3<float> viewPoint(resultView.X + pivot.X, resultView.Y + pivot.Y, resultView.Z + pivot.Z);
        view = position.vector(viewPoint).normalize();

        //moves up vector
        Quaternion<float> quatUp(up.X, up.Y, up.Z, 0.0f);
        const Quaternion<float>& resultUp = (quatRotation * quatUp.normalize()) * quatRotation.conjugate();
        up.X = resultUp.X;
        up.Y = resultUp.Y;
        up.Z = resultUp.Z;

        //moves position point
        if (std::fabs(distance) > std::numeric_limits<float>::epsilon()) {
            Vector3<float> axis = pivot.vector(position);
            Quaternion<float> quatPosition(axis.X, axis.Y, axis.Z, 0.0f);
            const Quaternion<float>& resultPosition = (quatRotation * quatPosition.normalize()) * quatRotation.conjugate();

            position.X = resultPosition.X + pivot.X;
            position.Y = resultPosition.Y + pivot.Y;
            position.Z = resultPosition.Z + pivot.Z;
        }

        updateViewMatrix();
    }

    bool Camera::onKeyPress(unsigned int) {
        //do nothing
        return true;
    }

    bool Camera::onKeyRelease(unsigned int) {
        //do nothing
        return true;
    }

    bool Camera::onMouseMove(double mouseX, double mouseY) {
        if (bUseMouse) {
            if (previousMouseX == std::numeric_limits<double>::max() && previousMouseY == std::numeric_limits<double>::max()) {
                previousMouseX = mouseX;
                previousMouseY = mouseY;
                return false;
            } else if (mouseX == previousMouseX && mouseY == previousMouseY) {
                return false;
            }

            Vector2<float> mouseDirection;
            mouseDirection.X = (float)(previousMouseX - mouseX) * MOUSE_SENSITIVITY_FACTOR * mouseSensitivityPercentage;
            mouseDirection.Y = (float)(previousMouseY - mouseY) * MOUSE_SENSITIVITY_FACTOR * mouseSensitivityPercentage;
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

            //rotate around the y and x axis
            Vector3<float> localXAxis = up.crossProduct(view).normalize();
            rotate(Quaternion<float>(localXAxis, -mouseDirection.Y));
            rotate(Quaternion<float>(Vector3<float>(0.0f, 1.0f, 0.0f), mouseDirection.X));

            updateViewMatrix();

            previousMouseX = mouseX;
            previousMouseY = mouseY;

            return false;
        }
        return true;
    }

    void Camera::updateViewMatrix() {
        //gluLookAt:
        const Vector3<float>& f = view;
        const Vector3<float>& s = f.crossProduct(up).normalize();
        const Vector3<float>& u = s.crossProduct(f).normalize();

        Matrix4<float> M(
                s[0], s[1], s[2], 0,
                u[0], u[1], u[2], 0,
                -f[0], -f[1], -f[2], 0,
                0, 0, 0, 1);

        Matrix4<float> eye;
        eye.buildTranslation(-position.X, -position.Y, -position.Z);
        mView = M * eye;

        frustum = baseFrustum * mView.inverse();
    }

}
