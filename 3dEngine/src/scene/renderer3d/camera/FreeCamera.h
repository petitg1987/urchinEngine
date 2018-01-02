#ifndef URCHINENGINE_FREECAMERA_H
#define URCHINENGINE_FREECAMERA_H

#include "Camera.h"

namespace urchin
{
	
	/**
	 * A free camera:
	 * - mouse right click released: directional keys allow to move camera forward/backward and rotate camera
	 * - mouse right click pressed: camera rotate based on mouse movements and directional keys allow to move camera forward/backward/left/right
	 */
	class FreeCamera : public Camera
	{
		public:
			FreeCamera(float, float, float);
			virtual ~FreeCamera() = default;
		
			void setKeys(unsigned int, unsigned int, unsigned int, unsigned int);
			void setSpeed(float, float);
		
			void onKeyDown(unsigned int);
			void onKeyUp(unsigned int);

			void updateCameraView(float);
		
		private:
			unsigned int keyFront, keyBack, keyLeft, keyRight;
			bool isKeyFrontPressed, isKeyBackPressed, isKeyLeftPressed, isKeyRightPressed;
			float speed, rotateSpeed;
	};
	
}

#endif
