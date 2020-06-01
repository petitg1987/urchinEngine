#include "MouseController.h"
#include "scene/SceneDisplayerWidget.h"

namespace urchin
{
    MouseController::MouseController(SceneDisplayerWidget *sceneDisplayerWidget) :
            sceneDisplayerWidget(sceneDisplayerWidget)
    {

    }

    void MouseController::moveMouse(unsigned int x, unsigned int y)
    {
        QCursor::setPos(sceneDisplayerWidget->mapToGlobal(QPoint((int)x, (int)y)));
    }
}
