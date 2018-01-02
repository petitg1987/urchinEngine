#include "FpsCamera.h"

#define DEFAULT_KEY_FRONT 'Z'
#define DEFAULT_KEY_BACK 'S'
#define DEFAULT_KEY_LEFT 'Q'
#define DEFAULT_KEY_RIGHT 'D'
#define DEFAULT_SPEED 10.0

namespace urchin
{
	
	/**
	* @param angle Angle of the field of view (fovy)
	*/
	FpsCamera::FpsCamera(float angle, float nearPlane, float farPlane) :
			Camera(angle, nearPlane, farPlane),
			keyFront(DEFAULT_KEY_FRONT),
			keyBack(DEFAULT_KEY_BACK),
			keyLeft(DEFAULT_KEY_LEFT),
			keyRight(DEFAULT_KEY_RIGHT),
			isKeyFrontPressed(false),
			isKeyBackPressed(false),
			isKeyLeftPressed(false),
			isKeyRightPressed(false),
			speed(DEFAULT_SPEED)
	{
		useMouseToMoveCamera(true);
	}

	/**
	* Specifies the keys to move the camera
	*/
	void FpsCamera::setKeys(unsigned int keyFront, unsigned int keyBack, unsigned int keyLeft, unsigned int keyRight)
	{
		this->keyFront = keyFront;
		this->keyBack = keyBack;
		this->keyLeft = keyLeft;
		this->keyRight = keyRight;
	}

	void FpsCamera::setSpeed(float speed)
	{
		this->speed = speed;
	}

	void FpsCamera::onKeyDown(unsigned int key)
	{
		if(key == keyFront)
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

	void FpsCamera::onKeyUp(unsigned int key)
	{
		if(key == keyFront)
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

	void FpsCamera::updateCameraView(float invFrameRate)
	{
		if(isKeyLeftPressed)
		{
			moveX(invFrameRate*speed);
		}
		
		if(isKeyRightPressed)
		{
			moveX(-invFrameRate*speed);
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
