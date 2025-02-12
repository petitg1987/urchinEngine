#include <scene/object/move/ObjectMoveController.h>

namespace urchin {
    ObjectMoveController::ObjectMoveController(Scene& scene, SceneController& sceneController, MouseController& mouseController,
                                               StatusBarController& statusBarController) :
            sceneWidth(0),
            sceneHeight(0),
            objectMoveAxisDisplayer(ObjectMoveAxisDisplayer(scene)),
            scene(scene),
            sceneController(sceneController),
            mouseController(mouseController),
            statusBarController(statusBarController),
            selectedObjectEntity(nullptr),
            selectedAxis(-1),
            oldMouseX(-1.0),
            oldMouseY(-1.0) {

    }

    void ObjectMoveController::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;
    }

    void ObjectMoveController::onCtrlXYZ(unsigned int axisIndex) {
        if (selectedObjectEntity) {
            if (selectedAxis == -1) {
                savedPosition = selectedObjectEntity->getModel()->getTransform().getPosition();
            }

            selectedAxis = (int) axisIndex;
            statusBarController.applyState(MODEL_MOVE);
        }
    }

    bool ObjectMoveController::onMouseMove(double mouseX, double mouseY) {
        bool propagateEvent = true;

        //Ignore the mouse event because missing old mouse information
        if (oldMouseX <= -1.0 && oldMouseY <= -1.0) {
            oldMouseX = mouseX;
            oldMouseY = mouseY;
            return propagateEvent;
        }

        //Ignore the mouse event received because the event has been created before we adjusted the mouse position (MouseController::moveMouse)
        const Camera& camera = scene.getActiveRenderer3d()->getCamera();
        if (std::abs(mouseX - oldMouseX) > (camera.getSceneWidth() / 2.0) || std::abs(mouseY - oldMouseY) > (camera.getSceneHeight() / 2.0)) {
            oldMouseX = mouseX;
            oldMouseY = mouseY;
            return propagateEvent;
        }

        bool mousePositionAdjusted = false;
        if (selectedAxis != -1) {
            if (!camera.isUseMouseToMoveCamera()) {
                mousePositionAdjusted = adjustMousePosition();
                if (!mousePositionAdjusted) {
                    moveObject(Point2((float)oldMouseX, (float)oldMouseY), Point2((float)mouseX, (float)mouseY));
                }
            }
            propagateEvent = false;
        }

        if (!mousePositionAdjusted) {
            oldMouseX = mouseX;
            oldMouseY = mouseY;
        }
        return propagateEvent;
    }

    void ObjectMoveController::onMouseOut() {
        if (selectedAxis != -1) {
            adjustMousePosition();
        }
    }

    bool ObjectMoveController::adjustMousePosition() {
        constexpr double SAFETY_MARGIN = 5.0;

        Point2<double> mousePosition = mouseController.getMousePosition();
        Point2<double> newMousePosition = mousePosition;

        if (mousePosition.X >= (double)sceneWidth - SAFETY_MARGIN) {
            newMousePosition.X = SAFETY_MARGIN;
        } else if (mousePosition.X <= SAFETY_MARGIN) {
            newMousePosition.X = (double)sceneWidth - SAFETY_MARGIN;
        }

        if (mousePosition.Y >= (double)sceneHeight - SAFETY_MARGIN) {
            newMousePosition.Y = SAFETY_MARGIN;
        } else if (mousePosition.Y <= SAFETY_MARGIN) {
            newMousePosition.Y = (double)sceneHeight - SAFETY_MARGIN;
        }

        if (mousePosition != newMousePosition) {
            oldMouseX = newMousePosition.X;
            oldMouseY = newMousePosition.Y;

            mouseController.moveMouse(newMousePosition.X, newMousePosition.Y);
            return true;
        }

        return false;
    }

    void ObjectMoveController::moveObject(const Point2<float>& oldMouseCoord, const Point2<float>& newMouseCoord) {
        constexpr float MOVE_SPEED_FACTOR = 20.0f;

        Point3<float> modelPosition = selectedObjectEntity->getModel()->getTransform().getPosition();
        CameraSpaceService cameraSpaceService(scene.getActiveRenderer3d()->getCamera());

        Point3<float> startAxisWorldSpacePoint = modelPosition;
        startAxisWorldSpacePoint[(unsigned int)selectedAxis] -= 1.0f;
        Point3<float> startAxisPointNdcSpace = cameraSpaceService.worldSpacePointToViewSpace(startAxisWorldSpacePoint);

        Point3<float> endAxisWorldSpacePoint = modelPosition;
        endAxisWorldSpacePoint[(unsigned int)selectedAxis] += 1.0f;
        Point3<float> endAxisPointNdcSpace = cameraSpaceService.worldSpacePointToViewSpace(endAxisWorldSpacePoint);

        Point3<float> startMouseCoordViewSpace = cameraSpaceService.screenPointToViewSpace(oldMouseCoord);
        Point3<float> endMouseCoordViewSpace = cameraSpaceService.screenPointToViewSpace(newMouseCoord);

        Vector3<float> mouseVector = startMouseCoordViewSpace.vector(endMouseCoordViewSpace);
        Vector3<float> axisVector = startAxisPointNdcSpace.vector(endAxisPointNdcSpace);

        float moveFactor = axisVector.normalize().dotProduct(mouseVector);
        float moveSpeed = scene.getActiveRenderer3d()->getCamera().getPosition().distance(modelPosition);

        Point3<float> newPosition = modelPosition;
        newPosition[(unsigned int)selectedAxis] += moveFactor * moveSpeed * MOVE_SPEED_FACTOR;
        updateObjectPosition(newPosition);
    }

    void ObjectMoveController::updateObjectPosition(const Point3<float>& newPosition) {
        Transform<float> transform = selectedObjectEntity->getModel()->getTransform();
        transform.setPosition(newPosition);

        sceneController.getObjectController().updateObjectTransform(*selectedObjectEntity, transform);
        notifyObservers(this, OBJECT_MOVED);
    }

    bool ObjectMoveController::onMouseLeftButton() {
        bool propagateEvent = true;

        if (selectedAxis != -1) {
            statusBarController.applyPreviousState();
            selectedAxis = -1;
            propagateEvent = false;
        }

        return propagateEvent;
    }

    bool ObjectMoveController::onEscapeKey() {
        bool propagateEvent = true;
        if (selectedAxis != -1) {
            updateObjectPosition(savedPosition);

            statusBarController.applyPreviousState();
            selectedAxis = -1;
            propagateEvent = false;
        }
        return propagateEvent;
    }

    void ObjectMoveController::setSelectedObjectEntity(const ObjectEntity* selectedObjectEntity) {
        this->selectedObjectEntity = selectedObjectEntity;
        this->selectedAxis = -1;

        if (selectedObjectEntity) {
            statusBarController.applyState(MODEL_SELECTED);
        } else {
            statusBarController.applyPreviousState();
        }
    }

    const ObjectEntity* ObjectMoveController::getSelectedObjectEntity() const {
        return selectedObjectEntity;
    }

    void ObjectMoveController::displayAxis() {
        if (selectedObjectEntity) {
            Point3<float> modelPosition = selectedObjectEntity->getModel()->getTransform().getPosition();
            objectMoveAxisDisplayer.displayAxis(modelPosition, (unsigned int)selectedAxis);
        } else {
            objectMoveAxisDisplayer.cleanCurrentDisplay();
        }
    }
}
