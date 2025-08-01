#include "scene/object/ObjectMoveController.h"

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
            lastMovedObjectEntity(nullptr),
            selectedAxis(-1),
            oldMouseX(-1.0),
            oldMouseY(-1.0) {

    }

    void ObjectMoveController::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;
    }

    void ObjectMoveController::onCtrlXYZ(unsigned int axisIndex) {
        if (!selectedObjectEntities.empty()) {
            if (selectedAxis == -1) {
                for (const ObjectEntity* selectedObjectEntity : selectedObjectEntities) {
                    savedPositions.insert({selectedObjectEntity, selectedObjectEntity->getModel()->getTransform().getPosition()});
                }
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
                    moveObjects(Point2((float)oldMouseX, (float)oldMouseY), Point2((float)mouseX, (float)mouseY));
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

    void ObjectMoveController::moveObjects(const Point2<float>& oldMouseCoord, const Point2<float>& newMouseCoord) {
        constexpr float MOVE_SPEED_FACTOR = 20.0f;

        Point3<float> modelsPosition = computeSelectObjectsMeanPosition();
        CameraSpaceService cameraSpaceService(scene.getActiveRenderer3d()->getCamera());

        Point3<float> startAxisWorldSpacePoint = modelsPosition;
        startAxisWorldSpacePoint[(unsigned int)selectedAxis] -= 1.0f;
        Point3<float> startAxisPointNdcSpace = cameraSpaceService.worldSpacePointToViewSpace(startAxisWorldSpacePoint);

        Point3<float> endAxisWorldSpacePoint = modelsPosition;
        endAxisWorldSpacePoint[(unsigned int)selectedAxis] += 1.0f;
        Point3<float> endAxisPointNdcSpace = cameraSpaceService.worldSpacePointToViewSpace(endAxisWorldSpacePoint);

        Point3<float> startMouseCoordViewSpace = cameraSpaceService.screenPointToViewSpace(oldMouseCoord);
        Point3<float> endMouseCoordViewSpace = cameraSpaceService.screenPointToViewSpace(newMouseCoord);

        Vector3<float> mouseVector = startMouseCoordViewSpace.vector(endMouseCoordViewSpace);
        Vector3<float> axisVector = startAxisPointNdcSpace.vector(endAxisPointNdcSpace);

        float moveFactor = axisVector.normalize().dotProduct(mouseVector);
        float moveSpeed = scene.getActiveRenderer3d()->getCamera().getPosition().distance(modelsPosition);

        for (const ObjectEntity* selectedObjectEntity : selectedObjectEntities) {
            Point3<float> newPosition = selectedObjectEntity->getModel()->getTransform().getPosition();
            newPosition[(unsigned int)selectedAxis] += moveFactor * moveSpeed * MOVE_SPEED_FACTOR;
            updateObjectPosition(selectedObjectEntity, newPosition);
        }
    }

    Point3<float> ObjectMoveController::computeSelectObjectsMeanPosition() const {
        Point3<float> meanPosition;
        for (const ObjectEntity* selectedObjectEntity : selectedObjectEntities) {
            meanPosition += selectedObjectEntity->getModel()->getTransform().getPosition();
        }
        meanPosition /= (float)selectedObjectEntities.size();
        return meanPosition;
    }

    void ObjectMoveController::updateObjectPosition(const ObjectEntity* objectEntity, const Point3<float>& newPosition) {
        Transform<float> transform = objectEntity->getModel()->getTransform();
        transform.setPosition(newPosition);
        sceneController.getObjectController().updateObjectTransform(*objectEntity, transform);

        lastMovedObjectEntity = objectEntity;
        notifyObservers(this, OBJECT_MOVED);
    }

    bool ObjectMoveController::onMouseLeftButton() {
        bool propagateEvent = true;

        if (selectedAxis != -1) {
            statusBarController.applyState(MODEL_SELECTED);
            selectedAxis = -1;
            propagateEvent = false;
        }

        return propagateEvent;
    }

    bool ObjectMoveController::onEscapeKey() {
        bool propagateEvent = true;
        if (selectedAxis != -1) {
            for (const ObjectEntity* selectedObjectEntity : selectedObjectEntities) {
                updateObjectPosition(selectedObjectEntity, savedPositions.at(selectedObjectEntity));
            }

            statusBarController.applyState(MODEL_SELECTED);
            selectedAxis = -1;
            propagateEvent = false;
        }
        return propagateEvent;
    }

    void ObjectMoveController::setSelectedObjectEntities(std::vector<const ObjectEntity*> selectedObjectEntities) {
        this->selectedObjectEntities = std::move(selectedObjectEntities);
        this->selectedAxis = -1;

        if (!this->selectedObjectEntities.empty()) {
            statusBarController.applyState(MODEL_SELECTED);
        } else {
            statusBarController.applyState(MAP_LOADED);
        }
    }

    const ObjectEntity& ObjectMoveController::getLastMovedObjectEntity() const {
        assert(lastMovedObjectEntity);
        return *lastMovedObjectEntity;
    }

    void ObjectMoveController::displayAxis() {
        if (!selectedObjectEntities.empty()) {
            Point3<float> modelsPosition = computeSelectObjectsMeanPosition();
            objectMoveAxisDisplayer.displayAxis(modelsPosition, (unsigned int)selectedAxis);
        } else {
            objectMoveAxisDisplayer.cleanCurrentDisplay();
        }
    }
}
