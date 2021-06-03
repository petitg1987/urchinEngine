#include <scene/objects/move/ObjectMoveController.h>

namespace urchin {
    ObjectMoveController::ObjectMoveController(SceneManager* sceneManager, SceneController* sceneController,
                                               const std::unique_ptr<MouseController>& mouseController,
                                               const std::unique_ptr<StatusBarController>& statusBarController) :
            sceneWidth(0),
            sceneHeight(0),
            objectMoveAxisDisplayer(sceneManager),
            sceneManager(sceneManager),
            sceneController(sceneController),
            mouseController(mouseController),
            statusBarController(statusBarController),
            selectedSceneObject(nullptr),
            selectedAxis(-1),
            oldMouseX(-1),
            oldMouseY(-1) {

    }

    void ObjectMoveController::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;
    }

    void ObjectMoveController::onCtrlXYZ(unsigned int axisIndex) {
        if (selectedAxis == -1) {
            savedPosition = selectedSceneObject->getModel()->getTransform().getPosition();
        }

        selectedAxis = (int)axisIndex;
        statusBarController->applyState(StatusBarState::OBJECT_MOVE);
    }

    bool ObjectMoveController::onMouseMove(int mouseX, int mouseY) {
        bool propagateEvent = true;
        bool mousePositionAdjusted = false;
        if (selectedAxis != -1) {
            if (oldMouseX != -1 && oldMouseY != -1 && !isCameraMoved()) {
                mousePositionAdjusted = adjustMousePosition();
                if (!mousePositionAdjusted) {
                    moveObject(Point2<float>((float)oldMouseX, (float)oldMouseY), Point2<float>((float)mouseX, (float)mouseY));
                }
            }

            propagateEvent = false;
        }

        if (!mousePositionAdjusted) {
            oldMouseX = mouseX;
            oldMouseY = mouseY;
        }
        oldCameraViewMatrix = sceneManager->getActiveRenderer3d()->getCamera()->getViewMatrix();

        return propagateEvent;
    }

    void ObjectMoveController::onMouseOut() {
        if (selectedAxis != -1) {
            adjustMousePosition();
        }
    }

    bool ObjectMoveController::isCameraMoved() const {
        Camera* camera = sceneManager->getActiveRenderer3d()->getCamera();
        for (unsigned int i = 0; i < 16; ++i) {
            if (this->oldCameraViewMatrix(i) != camera->getViewMatrix()(i)) {
                return true;
            }
        }
        return false;
    }

    bool ObjectMoveController::adjustMousePosition() {
        Point2<unsigned int> mousePosition = mouseController->getMousePosition();
        Point2<unsigned int> newMousePosition = mousePosition;

        if (mousePosition.X >= sceneWidth) {
            newMousePosition.X = 1;
        } else if (mousePosition.X <= 0) {
            newMousePosition.X = sceneWidth - 1;
        }

        if (mousePosition.Y >= sceneHeight) {
            newMousePosition.Y = 1;
        } else if (mousePosition.Y <= 0) {
            newMousePosition.Y = sceneHeight - 1;
        }

        if (mousePosition != newMousePosition) {
            oldMouseX = (int)newMousePosition.X;
            oldMouseY = (int)newMousePosition.Y;

            mouseController->moveMouse(newMousePosition.X, newMousePosition.Y);
            return true;
        }

        return false;
    }

    void ObjectMoveController::moveObject(const Point2<float>& oldMouseCoord, const Point2<float>& newMouseCoord) {
        Point3<float> objectPosition = selectedSceneObject->getModel()->getTransform().getPosition();
        CameraSpaceService cameraSpaceService(sceneManager->getActiveRenderer3d()->getCamera());

        Point3<float> startAxisWorldSpacePoint = objectPosition;
        startAxisWorldSpacePoint[(unsigned int)selectedAxis] -= 1.0f;
        Point2<float> startAxisPointScreenSpace = cameraSpaceService.worldSpacePointToScreenSpace(startAxisWorldSpacePoint);

        Point3<float> endAxisWorldSpacePoint = objectPosition;
        endAxisWorldSpacePoint[(unsigned int)selectedAxis] += 1.0f;
        Point2<float> endAxisPointScreenSpace = cameraSpaceService.worldSpacePointToScreenSpace(endAxisWorldSpacePoint);

        Vector2<float> mouseVector = oldMouseCoord.vector(newMouseCoord);
        Vector2<float> axisVector = startAxisPointScreenSpace.vector(endAxisPointScreenSpace);

        float moveFactor = axisVector.normalize().dotProduct(mouseVector);
        float moveSpeed = sceneManager->getActiveRenderer3d()->getCamera()->getPosition().distance(objectPosition);
        float moveReduceFactor = 0.001f;

        Point3<float> newPosition = objectPosition;
        newPosition[(unsigned int)selectedAxis] += moveFactor * moveSpeed * moveReduceFactor;
        updateObjectPosition(newPosition);
    }

    void ObjectMoveController::updateObjectPosition(const Point3<float>& newPosition) {
        Transform<float> transform = selectedSceneObject->getModel()->getTransform();
        transform.setPosition(newPosition);

        sceneController->getObjectController()->updateSceneObjectTransform(selectedSceneObject, transform);
        notifyObservers(this, NotificationType::OBJECT_MOVED);
    }

    bool ObjectMoveController::onMouseLeftButton() {
        bool propagateEvent = true;

        if (selectedAxis != -1) {
            statusBarController->applyPreviousState();
            selectedAxis = -1;
            propagateEvent = false;
        }

        return propagateEvent;
    }

    bool ObjectMoveController::onEscapeKey() {
        bool propagateEvent = true;
        if (selectedAxis != -1) {
            updateObjectPosition(savedPosition);

            statusBarController->applyPreviousState();
            selectedAxis = -1;
            propagateEvent = false;
        }
        return propagateEvent;
    }

    void ObjectMoveController::setSelectedSceneObject(const SceneObject* selectedSceneObject) {
        this->selectedSceneObject = selectedSceneObject;
        this->selectedAxis = -1;

        if (selectedSceneObject) {
            statusBarController->applyState(StatusBarState::OBJECT_SELECTED);
        } else {
            statusBarController->applyPreviousState();
        }
    }

    const SceneObject* ObjectMoveController::getSelectedSceneObject() const {
        return selectedSceneObject;
    }

    void ObjectMoveController::displayAxis() {
        if (selectedSceneObject) {
            Point3<float> modelPosition = selectedSceneObject->getModel()->getTransform().getPosition();
            objectMoveAxisDisplayer.displayAxis(modelPosition, (unsigned int)selectedAxis);
        } else {
            objectMoveAxisDisplayer.cleanCurrentDisplay();
        }
    }
}
