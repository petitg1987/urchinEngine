#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include "UrchinCommon.h"

namespace urchin
{

	class Camera
	{
		public:
			Camera(float, float, float);
			virtual ~Camera();
			
			void onResize(int, int);
			virtual void moveMouse(int, int) = 0;
			
			void useMouseToMoveCamera(bool);
			bool isUseMouseToMoveCamera() const;
			void setMouseSensitivity(float);
			void setDistance(float);
			void setMaxRotationX(float);
			
			const Matrix4<float> &getViewMatrix() const;
			const Matrix4<float> &getProjectionMatrix() const;
			const Frustum<float> &getFrustum() const;

			const Point3<float> &getPosition() const;
			const Point3<float> &getView() const;
			const Vector3<float> &getUp() const;
		
			void moveTo(const Point3<float> &);
			void moveX(float);
			void moveZ(float);
			void rotate(float, float, float, float);
			
			virtual void onKeyDown(unsigned int);
			virtual void onKeyUp(unsigned int);
			virtual void updateCameraView(float) = 0;
		
			void onMouseMove(int, int);
			void onKeyboardLocked(bool);

		protected:
			bool isKeyboardLocked() const;

		private:
			void updateMatrix();

			Matrix4<float> mView, mProjection;

			float angle, nearPlane, farPlane;
			Frustum<float> baseFrustum; //base frustum (without any matrix transformation)
			Frustum<float> frustum;

			Point3<float> position, view;
			Vector3<float> up;
			float currentRotationX, maxRotationX;
			float distance; //distance between the camera and the rotation point (0 : first person camera | >0 : third person camera)
		
			bool bUseMouse; //true if the cursor is used to move the camera
			bool keyboardLocked; //true if the keyboard is locked by another resource
			float mouseSensitivity;
			int middleScreenX, middleScreenY, oldMouseX, oldMouseY;
	};

}

#endif
