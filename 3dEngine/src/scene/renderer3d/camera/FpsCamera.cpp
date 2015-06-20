#include "UrchinCommon.h"

#include "FpsCamera.h"

namespace urchin
{
	
	/**
	* @param angle Angle of the field of view (fovy)
	*/
	FpsCamera::FpsCamera(float angle, float nearPlane, float farPlane) :
			Camera(angle, nearPlane, farPlane),
			isKeyFrontPressed(false),
			isKeyBackPressed(false),
			isKeyLeftPressed(false),
			isKeyRightPressed(false)
	{
		keyFront = ConfigService::instance()->getCharValue("fpsCamera.defaultValue.keyFront");
		keyBack = ConfigService::instance()->getCharValue("fpsCamera.defaultValue.keyBack");
		keyLeft = ConfigService::instance()->getCharValue("fpsCamera.defaultValue.keyLeft");
		keyRight = ConfigService::instance()->getCharValue("fpsCamera.defaultValue.keyRight");

		speed = ConfigService::instance()->getFloatValue("fpsCamera.defaultValue.speed");
		
		useMouseToMoveCamera(true);
	}

	FpsCamera::~FpsCamera()
	{
		
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
		if(isKeyboardLocked())
		{
			return;
		}

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
		if(isKeyboardLocked())
		{
			return;
		}

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
