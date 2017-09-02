#ifndef URCHINENGINE_MAPEDITOR_SCENEFREECAMERA_H
#define URCHINENGINE_MAPEDITOR_SCENEFREECAMERA_H

#include <QWidget>
#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

namespace urchin
{

	class SceneFreeCamera : public FreeCamera
	{
		public:
			SceneFreeCamera(float, float, float, const QWidget *);
			virtual ~SceneFreeCamera() = default;

			void moveMouse(unsigned int, unsigned int);

		private:
			const QWidget *parentWidget;
	};

}

#endif
