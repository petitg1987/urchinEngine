#include "MouseController.h"
#include "scene/displayer/SceneDisplayerWidget.h"

namespace urchin
{
    MouseController::MouseController(SceneDisplayerWidget *sceneDisplayerWidget) :
            sceneDisplayerWidget(sceneDisplayerWidget)
    {

    }

    void MouseController::moveMouse(unsigned int x, unsigned int y)
    {
        sceneDisplayerWidget->disableNextMouseMoveEvent();
        QCursor::setPos(sceneDisplayerWidget->mapToGlobal(QPoint((int)x, (int)y)));
    }
}
