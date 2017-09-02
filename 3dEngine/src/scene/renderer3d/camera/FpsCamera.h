#ifndef URCHINENGINE_FPSCAMERA_H
#define URCHINENGINE_FPSCAMERA_H

#include "Camera.h"

namespace urchin
{
	
	/**
	 * A FPS camera:
	 * - mouse allow to rotate camera based on mouse movements
	 * - directional keys allow to move camera forward/backward/left/right
	 */
	class FpsCamera : public Camera
	{
		public:
			FpsCamera(float, float, float);
			virtual ~FpsCamera();
		
			void setKeys(unsigned int, unsigned int, unsigned int, unsigned int);
			void setSpeed(float);

			void onKeyDown(unsigned int);
			void onKeyUp(unsigned int);
		
			void updateCameraView(float);
			
		private:
			unsigned int keyFront, keyBack, keyLeft, keyRight;
			bool isKeyFrontPressed, isKeyBackPressed, isKeyLeftPressed, isKeyRightPressed;
			float speed;
	};

}

#endif
