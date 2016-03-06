#include "UrchinCommon.h"

#include "FreeCamera.h"
#include "scene/SceneManager.h"

#define DEFAULT_KEY_FRONT 'Z'
#define DEFAULT_KEY_BACK 'S'
#define DEFAULT_KEY_LEFT 'Q'
#define DEFAULT_KEY_RIGHT 'D'
#define DEFAULT_SPEED 10.0
#define DEFAULT_ROTATE_SPEED 2.0

namespace urchin
{
	
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
			rotateSpeed(DEFAULT_ROTATE_SPEED)
	{

	}

	FreeCamera::~FreeCamera()
	{
		
	}

	/**
	* Specifies the keys to move the camera
	*/
	void FreeCamera::setKeys(unsigned int keyFront, unsigned int keyBack, unsigned int keyLeft, unsigned int keyRight)
	{
		this->keyFront = keyFront;
		this->keyBack = keyBack;
		this->keyLeft = keyLeft;
		this->keyRight = keyRight;
	}

	void FreeCamera::setSpeed(float speed, float rotateSpeed)
	{
		this->speed = speed;
		this->rotateSpeed = rotateSpeed;
	}

	void FreeCamera::onKeyDown(unsigned int key)
	{
		if(key == KEY_MOUSE_RIGHT)
		{
			useMouseToMoveCamera(true);
		}else if(key == keyFront)
		{
			isKeyFrontPressed = true;
		}else if(key == keyBack)
		{
			isKeyBackPressed = true;
		}else if(key == keyLeft)
		{
			isKeyLeftPressed = true;
		}else if(key == keyRight)
		{
			isKeyRightPressed = true;
		}
	}

	void FreeCamera::onKeyUp(unsigned int key)
	{
		if(key == KEY_MOUSE_RIGHT)
		{
			useMouseToMoveCamera(false);
		}else if(key == keyFront)
		{
			isKeyFrontPressed = false;
		}else if(key == keyBack)
		{
			isKeyBackPressed = false;
		}else if(key == keyLeft)
		{
			isKeyLeftPressed = false;
		}else if(key == keyRight)
		{
			isKeyRightPressed = false;
		}
	}

	void FreeCamera::updateCameraView(float invFrameRate)
	{
		if(isKeyLeftPressed)
		{
			if(isUseMouseToMoveCamera())
			{
				moveX(invFrameRate*speed);
			}else
			{
				rotate(invFrameRate*rotateSpeed, 0.0, 1.0, 0.0);
			}
		}
		
		if(isKeyRightPressed)
		{
			if(isUseMouseToMoveCamera())
			{
				moveX(-invFrameRate*speed);
			}else
			{
				rotate(-invFrameRate*rotateSpeed, 0.0, 1.0, 0.0);
			}
		}
		
		if(isKeyFrontPressed)
		{
			moveZ(invFrameRate*speed);
		}
		
		if(isKeyBackPressed)
		{
			moveZ(-invFrameRate*speed);
		}
	}

}
