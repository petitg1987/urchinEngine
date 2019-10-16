#include <QCursor>

#include "SceneFreeCamera.h"

namespace urchin
{

	SceneFreeCamera::SceneFreeCamera(float angle, float nearPlane, float farPlane, const QWidget *parentWidget) :
		FreeCamera(angle, nearPlane, farPlane),
		parentWidget(parentWidget)
	{

	}

	void SceneFreeCamera::moveMouse(unsigned int x, unsigned int y)
	{
		QPoint point((int)x, (int)y);
		QCursor::setPos(parentWidget->mapToGlobal(point));
	}

}
