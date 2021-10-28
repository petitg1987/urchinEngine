#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class Camera : public Observable {
        public:
            Camera(float, float, float);
            ~Camera() override = default;

            enum NotificationType {
                PROJECTION_UPDATE
            };

            void initialize(unsigned int, unsigned int);
            void onResize(unsigned int, unsigned int);
            unsigned int getSceneWidth() const;
            unsigned int getSceneHeight() const;

            void resetPreviousMousePosition(double previousMouseX = std::numeric_limits<double>::max(), double previousMouseY = std::numeric_limits<double>::max());
            void useMouseToMoveCamera(bool);
            bool isUseMouseToMoveCamera() const;
            void setMouseSensitivityPercentage(float);
            void setInvertYAxis(bool);
            void setDistance(float);
            bool isFirstPersonCamera() const;
            void setMaxRotationX(float);

            const Matrix4<float>& getViewMatrix() const;
            const Matrix4<float>& getProjectionMatrix() const;

            const Point3<float>& getPosition() const;
            const Vector3<float>& getView() const;
            const Vector3<float>& getUp() const;

            const Quaternion<float>& getOrientation() const;

            float getHorizontalFovAngle() const;
            void updateHorizontalFovAngle(float);
            float computeFov() const;
            float getNearPlane() const;
            float getFarPlane() const;
            const Frustum<float>& getFrustum() const;

            void moveTo(const Point3<float>&);
            void moveOnLocalXAxis(float);
            void moveOnLocalZAxis(float);
            void rotate(const Quaternion<float>&);

            virtual bool onKeyPress(unsigned int);
            virtual bool onKeyRelease(unsigned int);
            virtual void updateCameraView(float) = 0;

            virtual bool onMouseMove(double, double);

        private:
            void initializeOrUpdate(unsigned int, unsigned int);
            float computeVerticalFovAngle() const;
            float computeFov(float) const;
            void updateComponents();

            const float MOUSE_SENSITIVITY_FACTOR;
            Matrix4<float> mView, mProjection;

            Point3<float> position;
            Vector3<float> view, up;

            Quaternion<float> orientation;

            float horizontalFovAngle, nearPlane, farPlane;
            Frustum<float> baseFrustum; //base frustum (without any matrix transformation)
            Frustum<float> frustum;

            float maxRotationX;
            float distance; //distance between the camera and the rotation point (0 : first person camera | >0 : third person camera)
            bool bUseMouse; //true if the cursor is used to move the camera
            float mouseSensitivityPercentage;
            bool invertYAxis;
            unsigned int sceneWidth, sceneHeight;
            double previousMouseX, previousMouseY;
    };

}
