#ifndef ENGINE_DEBUGVISUALIZER_SCENEFREECAMERA_H
#define ENGINE_DEBUGVISUALIZER_SCENEFREECAMERA_H

#include <QWidget>
#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

namespace urchin
{

	class SceneFreeCamera : public FreeCamera
	{
		public:
			SceneFreeCamera(float, float, float, const QWidget *);
			virtual ~SceneFreeCamera();

			void moveMouse(unsigned int, unsigned int);

		private:
			const QWidget *parentWidget;
	};

}

#endif
