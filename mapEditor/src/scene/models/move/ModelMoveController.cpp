#include <scene/models/move/ModelMoveController.h>

namespace urchin {
    ModelMoveController::ModelMoveController(Scene& scene, SceneController& sceneController, MouseController& mouseController,
                                               StatusBarController& statusBarController) :
            sceneWidth(0),
            sceneHeight(0),
            modelMoveAxisDisplayer(ModelMoveAxisDisplayer(scene)),
            scene(scene),
            sceneController(sceneController),
            mouseController(mouseController),
            statusBarController(statusBarController),
            selectedSceneModel(nullptr),
            selectedAxis(-1),
            oldMouseX(-1),
            oldMouseY(-1) {

    }

    void ModelMoveController::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;
    }

    void ModelMoveController::onCtrlXYZ(unsigned int axisIndex) {
        if (selectedAxis == -1) {
            savedPosition = selectedSceneModel->getModel()->getTransform().getPosition();
        }

        selectedAxis = (int)axisIndex;
        statusBarController.applyState(StatusBarState::MODEL_MOVE);
    }

    bool ModelMoveController::onMouseMove(int mouseX, int mouseY) {
        bool propagateEvent = true;
        bool mousePositionAdjusted = false;
        if (selectedAxis != -1) {
            if (oldMouseX != -1 && oldMouseY != -1 && !isCameraMoved()) {
                mousePositionAdjusted = adjustMousePosition();
                if (!mousePositionAdjusted) {
                    moveModel(Point2<float>((float)oldMouseX, (float)oldMouseY), Point2<float>((float)mouseX, (float)mouseY));
                }
            }

            propagateEvent = false;
        }

        if (!mousePositionAdjusted) {
            oldMouseX = mouseX;
            oldMouseY = mouseY;
        }
        oldCameraViewMatrix = scene.getActiveRenderer3d()->getCamera()->getViewMatrix();

        return propagateEvent;
    }

    void ModelMoveController::onMouseOut() {
        if (selectedAxis != -1) {
            adjustMousePosition();
        }
    }

    bool ModelMoveController::isCameraMoved() const {
        Camera* camera = scene.getActiveRenderer3d()->getCamera();
        for (unsigned int i = 0; i < 16; ++i) {
            if (this->oldCameraViewMatrix(i) != camera->getViewMatrix()(i)) {
                return true;
            }
        }
        return false;
    }

    bool ModelMoveController::adjustMousePosition() {
        Point2<int> mousePosition = mouseController.getMousePosition();
        Point2<int> newMousePosition = mousePosition;

        if (mousePosition.X >= (int)sceneWidth) {
            newMousePosition.X = 1;
        } else if (mousePosition.X <= 0) {
            newMousePosition.X = (int)sceneWidth - 1;
        }

        if (mousePosition.Y >= (int)sceneHeight) {
            newMousePosition.Y = 1;
        } else if (mousePosition.Y <= 0) {
            newMousePosition.Y = (int)sceneHeight - 1;
        }

        if (mousePosition != newMousePosition) {
            oldMouseX = (int)newMousePosition.X;
            oldMouseY = (int)newMousePosition.Y;

            mouseController.moveMouse(newMousePosition.X, newMousePosition.Y);
            return true;
        }

        return false;
    }

    void ModelMoveController::moveModel(const Point2<float>& oldMouseCoord, const Point2<float>& newMouseCoord) {
        Point3<float> modelPosition = selectedSceneModel->getModel()->getTransform().getPosition();
        CameraSpaceService cameraSpaceService(scene.getActiveRenderer3d()->getCamera());

        Point3<float> startAxisWorldSpacePoint = modelPosition;
        startAxisWorldSpacePoint[(unsigned int)selectedAxis] -= 1.0f;
        Point2<float> startAxisPointScreenSpace = cameraSpaceService.worldSpacePointToScreenSpace(startAxisWorldSpacePoint);

        Point3<float> endAxisWorldSpacePoint = modelPosition;
        endAxisWorldSpacePoint[(unsigned int)selectedAxis] += 1.0f;
        Point2<float> endAxisPointScreenSpace = cameraSpaceService.worldSpacePointToScreenSpace(endAxisWorldSpacePoint);

        Vector2<float> mouseVector = oldMouseCoord.vector(newMouseCoord);
        Vector2<float> axisVector = startAxisPointScreenSpace.vector(endAxisPointScreenSpace);

        float moveFactor = axisVector.normalize().dotProduct(mouseVector);
        float moveSpeed = scene.getActiveRenderer3d()->getCamera()->getPosition().distance(modelPosition);
        float moveReduceFactor = 0.001f;

        Point3<float> newPosition = modelPosition;
        newPosition[(unsigned int)selectedAxis] += moveFactor * moveSpeed * moveReduceFactor;
        updateModelPosition(newPosition);
    }

    void ModelMoveController::updateModelPosition(const Point3<float>& newPosition) {
        Transform<float> transform = selectedSceneModel->getModel()->getTransform();
        transform.setPosition(newPosition);

        sceneController.getModelController().updateSceneModelTransform(*selectedSceneModel, transform);
        notifyObservers(this, NotificationType::MODEL_MOVED);
    }

    bool ModelMoveController::onMouseLeftButton() {
        bool propagateEvent = true;

        if (selectedAxis != -1) {
            statusBarController.applyPreviousState();
            selectedAxis = -1;
            propagateEvent = false;
        }

        return propagateEvent;
    }

    bool ModelMoveController::onEscapeKey() {
        bool propagateEvent = true;
        if (selectedAxis != -1) {
            updateModelPosition(savedPosition);

            statusBarController.applyPreviousState();
            selectedAxis = -1;
            propagateEvent = false;
        }
        return propagateEvent;
    }

    void ModelMoveController::setSelectedSceneModel(const SceneModel* selectedSceneModel) {
        this->selectedSceneModel = selectedSceneModel;
        this->selectedAxis = -1;

        if (selectedSceneModel) {
            statusBarController.applyState(StatusBarState::MODEL_SELECTED);
        } else {
            statusBarController.applyPreviousState();
        }
    }

    const SceneModel* ModelMoveController::getSelectedSceneModel() const {
        return selectedSceneModel;
    }

    void ModelMoveController::displayAxis() {
        if (selectedSceneModel) {
            Point3<float> modelPosition = selectedSceneModel->getModel()->getTransform().getPosition();
            modelMoveAxisDisplayer.displayAxis(modelPosition, (unsigned int)selectedAxis);
        } else {
            modelMoveAxisDisplayer.cleanCurrentDisplay();
        }
    }
}