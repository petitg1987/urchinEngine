#ifndef URCHINENGINE_CAMERA_H
#define URCHINENGINE_CAMERA_H

#include "UrchinCommon.h"

#define DEFAULT_MAX_ROTATION_X 0.995
#define DEFAULT_MOUSE_SENSITIVITY 0.006

namespace urchin
{

	class Camera
	{
		public:
			Camera(float, float, float);
			virtual ~Camera() = default;
			
			void onResize(unsigned int, unsigned int);
			virtual void moveMouse(unsigned int, unsigned int) = 0;
			
			void useMouseToMoveCamera(bool);
			bool isUseMouseToMoveCamera() const;
			void setMouseSensitivity(float);
			void setDistance(float);
			void setMaxRotationX(float);
			
			const Matrix4<float> &getViewMatrix() const;
			const Matrix4<float> &getProjectionMatrix() const;
			const Frustum<float> &getFrustum() const;

			const Point3<float> &getPosition() const;
			const Vector3<float> &getView() const;
			const Vector3<float> &getUp() const;

			float getSceneWidth();
			float getSceneHeight();
			float getAngle() const;
			float getNearPlane() const;
			float getFarPlane() const;

			void moveTo(const Point3<float> &);
			void moveOnLocalXAxis(float);
			void moveOnLocalZAxis(float);
			void lookAt(const Vector3<float> &);
			void rotate(const Quaternion<float> &);
			
			virtual bool onKeyDown(unsigned int);
			virtual bool onKeyUp(unsigned int);
			virtual void updateCameraView(float) = 0;
		
			bool onMouseMove(int, int);

		private:
			void updateViewMatrix();

			Matrix4<float> mView, mProjection;

			float angle, nearPlane, farPlane;
			Frustum<float> baseFrustum; //base frustum (without any matrix transformation)
			Frustum<float> frustum;

			Point3<float> position;
			Vector3<float> view, up;
			float maxRotationX;
			float distance; //distance between the camera and the rotation point (0 : first person camera | >0 : third person camera)

			bool bUseMouse; //true if the cursor is used to move the camera
			float mouseSensitivity;
			unsigned int sceneWidth, sceneHeight;
			unsigned int middleScreenX, middleScreenY;
			unsigned int oldMouseX, oldMouseY;
	};

}

#endif
