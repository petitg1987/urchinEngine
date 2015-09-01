#include <limits>

#include "Camera.h"

#define DEFAULT_MAX_ROTATION_X 0.995
#define DEFAULT_MOUSE_SENSITIVITY 0.006

namespace urchin
{

	/**
	* @param angle Angle of the field of view (fovy)
	*/
	Camera::Camera(float angle, float nearPlane, float farPlane) :
			mView(Matrix4<float>()),
			mProjection(Matrix4<float>()),
			angle(angle),
			nearPlane(nearPlane),
			farPlane(farPlane),
			baseFrustum(Frustum<float>(angle, 1.0, nearPlane, farPlane)),
			frustum(Frustum<float>(angle, 1.0, nearPlane, farPlane)),
			position(Point3<float>(0.0, 0.0, 0.0)),
			view(Point3<float>(0.0, 0.0, -1.0)),
			up(Vector3<float>(0.0, 1.0, 0.0)),
			currentRotationX(0.0),
			maxRotationX(DEFAULT_MAX_ROTATION_X),
			distance(0.0),
			bUseMouse(false),
			keyboardLocked(false),
			mouseSensitivity(DEFAULT_MOUSE_SENSITIVITY),
			middleScreenX(0),
			middleScreenY(0),
			oldMouseX(0),
			oldMouseY(0)
	{

	}

	Camera::~Camera()
	{

	}

	void Camera::onResize(int width, int height)
	{
		middleScreenX = width/2;
		middleScreenY = height/2;

		if(bUseMouse)
		{
			moveMouse(middleScreenX, middleScreenY);
		}

		//projection matrix
		float fov = 1.0/std::tan((angle*PI_VALUE)/360.0);
		float ratio = (float)width/(float)height;
		mProjection.setValues(
			fov/ratio, 	0, 		0, 		0,
			0, 				fov, 	0, 		0,
			0, 				0, 		(farPlane+nearPlane)/(nearPlane-farPlane),  (2*farPlane*nearPlane)/(nearPlane-farPlane),
			0, 				0, 		-1,		0);

		//frustum
		baseFrustum.buildFrustum(angle, ratio, nearPlane, farPlane);
		frustum = baseFrustum * mView.inverse();
	}

	void Camera::useMouseToMoveCamera(bool use)
	{
		if(use==true)
		{
			moveMouse(middleScreenX, middleScreenY);
		}else
		{
			moveMouse(oldMouseX, oldMouseY);
		}
		
		bUseMouse = use;
	}

	bool Camera::isUseMouseToMoveCamera() const
	{
		return bUseMouse;
	}

	/**
	 * @return True if keyboard is locked by another resource
	 */
	bool Camera::isKeyboardLocked() const
	{
		return keyboardLocked;
	}

	/**
	 * @param keyboardLocked Indicates if keyboard is locked by another resource
	 */
	void Camera::onKeyboardLocked(bool keyboardLocked)
	{
		this->keyboardLocked = keyboardLocked;
	}


	void Camera::setMouseSensitivity(float mouseSensitivity)
	{
		this->mouseSensitivity = mouseSensitivity;
	}

	/**
	* @param distance Distance between the camera and the rotation point (0 : first person camera | >0 : third person camera)
	*/
	void  Camera::setDistance(float distance)
	{
		this->distance = distance;
	}

	void  Camera::setMaxRotationX(float maxRotationX)
	{
		this->maxRotationX = maxRotationX;
	}

	const Matrix4<float> &Camera::getViewMatrix() const
	{
		return mView;
	}

	const Matrix4<float> &Camera::getProjectionMatrix() const
	{
		return mProjection;
	}

	const Frustum<float> &Camera::getFrustum() const
	{
		return frustum;
	}

	const Point3<float> &Camera::getPosition() const
	{	
		return position;
	}

	const Point3<float> &Camera::getView() const
	{
		return view;
	}

	const Vector3<float> &Camera::getUp() const
	{
		return up;
	}

	void Camera::moveTo(const Point3<float> &newPos)
	{
		Vector3<float> axis = position.vector(newPos);
		position = newPos;
		view = view.translate(axis);

		updateMatrix();
	}

	void Camera::moveZ(float dist)
	{
		Vector3<float> axis = position.vector(view).normalize();
		axis = axis * dist;

		position = position.translate(axis);
		view = view.translate(axis);

		updateMatrix();
	}

	void Camera::moveX(float dist)
	{
		Vector3<float> axis = (view.vector(position)).crossProduct(up).normalize();
		axis = axis * dist;

		position = position.translate(axis);
		view = view.translate(axis);

		updateMatrix();
	}

	/**
	* @param angle Angle of the rotation in radian
	* @param x X coordinate of the vector where the camera turns around
	* @param y Y coordinate of the vector where the camera turns around
	* @param z Z coordinate of the vector where the camera turns around
	*/
	void Camera::rotate(float angle, float x, float y, float z)
	{
		Point3<float> pivot;
		if(fabs(distance) > std::numeric_limits<float>::epsilon())
		{
			Vector3<float> axis = position.vector(view).normalize();
			pivot =  position.translate(axis * distance);
		}else
		{
			pivot = position;
		}

		Quaternion<float> rotationAxis(Vector3<float>(x, y, z), angle);

		//moves view point
		Vector3<float> axis = pivot.vector(view);
		Quaternion<float> quatView(axis.X, axis.Y, axis.Z, 0.0f);
		const Quaternion<float> &resultView = (rotationAxis * quatView.normalize()) * rotationAxis.conjugate();
		view.X = resultView.X + pivot.X;
		view.Y = resultView.Y + pivot.Y;
		view.Z = resultView.Z + pivot.Z;

		//moves up vector
		Quaternion<float> quatUp(up.X, up.Y, up.Z, 0.0f);
		const Quaternion<float> &resultUp = (rotationAxis * quatUp.normalize()) * rotationAxis.conjugate();
		up.X = resultUp.X;
		up.Y = resultUp.Y;
		up.Z = resultUp.Z;

		//moves position point
		if(fabs(distance) > std::numeric_limits<float>::epsilon())
		{
			axis = pivot.vector(position);
			Quaternion<float> quatPosition(axis.X, axis.Y, axis.Z, 0.0f);
			const Quaternion<float> &resultPosition = (rotationAxis * quatPosition.normalize()) * rotationAxis.conjugate();

			position.X = resultPosition.X + pivot.X;
			position.Y = resultPosition.Y + pivot.Y;
			position.Z = resultPosition.Z + pivot.Z;
		}

		updateMatrix();
	}

	void Camera::onKeyDown(unsigned int key)
	{
		//do nothing
	}

	void Camera::onKeyUp(unsigned int key)
	{
		//do nothing
	}

	void Camera::onMouseMove(int mouseX, int mouseY)
	{
		if(bUseMouse==false)
		{
			oldMouseX = mouseX;
			oldMouseY = mouseY;
			return;
		}

		if((mouseX == middleScreenX) && (mouseY == middleScreenY))
		{
			return;
		}

		//move the mouse back to the middle of the screen
		moveMouse(middleScreenX, middleScreenY);

		//vector that describes mouseposition - center
		Vector2<float> mouseDirection;
		mouseDirection.X = (middleScreenX - mouseX)*mouseSensitivity;
		mouseDirection.Y = (middleScreenY - mouseY)*mouseSensitivity;

		//we don't want to rotate up/down more than "MaxRotationX" percent
		Vector3<float> axis = position.vector(view).normalize();
		currentRotationX = Vector3<float>(0.0, 1.0, 0.0).dotProduct(axis);

		currentRotationX += mouseDirection.Y;
		if(currentRotationX > 0.0 && currentRotationX > maxRotationX)
		{
			mouseDirection.Y -= (currentRotationX - maxRotationX);
			currentRotationX = maxRotationX;
		}else if(currentRotationX < 0.0 && currentRotationX < -maxRotationX)
		{
			mouseDirection.Y -= (currentRotationX + maxRotationX);
			currentRotationX = -maxRotationX;
		}

		//get the axis to rotate around the x axis.
		axis = ((position.vector(view)).crossProduct(up)).normalize();

		//rotate around the y and x axis
		rotate(mouseDirection.Y, axis.X, axis.Y, axis.Z);
		rotate(mouseDirection.X, 0.0, 1.0, 0.0);

		updateMatrix();
	}

	void Camera::updateMatrix()
	{
		 //gluLookAt :
		const Vector3<float> &f = position.vector(view).normalize();
		const Vector3<float> &s = f.crossProduct(up).normalize();
		const Vector3<float> &u = s.crossProduct(f).normalize();

		Matrix4<float> M(
				s[0],	s[1],	s[2],	0,
				u[0], u[1],	u[2],	0,
				-f[0],	-f[1],	-f[2],	0,
				0,	0,	0,	1);

		Matrix4<float> eye;
		eye.buildTranslation(-position.X, -position.Y, -position.Z);
		mView = M * eye;

		frustum = baseFrustum * mView.inverse();
	}

}
