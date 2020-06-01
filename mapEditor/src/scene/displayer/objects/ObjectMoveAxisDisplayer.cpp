#include "ObjectMoveAxisDisplayer.h"

namespace urchin
{
    ObjectMoveAxisDisplayer::ObjectMoveAxisDisplayer(SceneManager *sceneManager, StatusBarController statusBarController) :
            sceneManager(sceneManager),
            statusBarController(std::move(statusBarController)),
            selectedSceneObject(nullptr),
            selectedAxis(-1),
            oldMouseX(-1),
            oldMouseY(-1)
    {

    }

    ObjectMoveAxisDisplayer::~ObjectMoveAxisDisplayer()
    {
        cleanCurrentDisplay();
    }

    void ObjectMoveAxisDisplayer::onCtrlXYZ(unsigned int axisIndex)
    {
        selectedAxis = axisIndex;
        statusBarController.applyState(StatusBarState::OBJECT_MOVE);
    }

    bool ObjectMoveAxisDisplayer::onMouseMove(int mouseX, int mouseY)
    {
        bool propagateEvent = true;
        if(selectedAxis != -1)
        {
            if(oldMouseX != -1 && oldMouseY != -1 && !isCameraMoved())
            {
                moveObject(Point2<float>(oldMouseX, oldMouseY), Point2<float>(mouseX, mouseY));
            }

            propagateEvent = false;
        }

        this->oldMouseX = mouseX;
        this->oldMouseY = mouseY;
        this->oldCameraViewMatrix = sceneManager->getActiveRenderer3d()->getCamera()->getViewMatrix();

        return propagateEvent;
    }

    bool ObjectMoveAxisDisplayer::isCameraMoved() const
    {
        Camera *camera = sceneManager->getActiveRenderer3d()->getCamera();
        for(unsigned int i=0; i<16; ++i)
        {
            if(this->oldCameraViewMatrix(i) != camera->getViewMatrix()(i))
            {
                return true;
            }
        }
        return false;
    }

    void ObjectMoveAxisDisplayer::moveObject(const Point2<float> &oldMouseCoord, const Point2<float> &newMouseCoord)
    {
        Point3<float> objectPosition = selectedSceneObject->getModel()->getTransform().getPosition();
        CameraSpaceService cameraSpaceService(sceneManager->getActiveRenderer3d()->getCamera());

        Point3<float> startAxisWorldSpacePoint = objectPosition;
        startAxisWorldSpacePoint[selectedAxis] -= 1.0f;
        Point2<float> startAxisPointScreenSpace = cameraSpaceService.worldSpacePointToScreenSpace(startAxisWorldSpacePoint);

        Point3<float> endAxisWorldSpacePoint = objectPosition;
        endAxisWorldSpacePoint[selectedAxis] += 1.0f;
        Point2<float> endAxisPointScreenSpace = cameraSpaceService.worldSpacePointToScreenSpace(endAxisWorldSpacePoint);

        Vector2<float> mouseVector = oldMouseCoord.vector(newMouseCoord);
        Vector2<float> axisVector = startAxisPointScreenSpace.vector(endAxisPointScreenSpace);

        float moveFactor = axisVector.normalize().dotProduct(mouseVector);
        float moveSpeed = sceneManager->getActiveRenderer3d()->getCamera()->getPosition().distance(objectPosition);
        float moveReduceFactor = 0.001f;

        Point3<float> newPosition = objectPosition;
        newPosition[selectedAxis] += moveFactor * moveSpeed * moveReduceFactor;
        updateObjectPosition(newPosition);
    }

    void ObjectMoveAxisDisplayer::updateObjectPosition(const Point3<float> &newPosition)
    { //TODO inform mapEditor of change on object to ask to save (see ObjectController::updateSceneObjectTransform)
        Transform<float> transform = selectedSceneObject->getModel()->getTransform();
        transform.setPosition(newPosition);

        selectedSceneObject->getModel()->setTransform(transform);
        if(selectedSceneObject->getRigidBody())
        {
            selectedSceneObject->getRigidBody()->setTransform(transform);
        }
    }

    bool ObjectMoveAxisDisplayer::onMouseLeftButton()
    {
        bool propagateEvent = true;

        if(selectedAxis != -1)
        {
            //TODO confirm ?

            statusBarController.applyPreviousState();
            selectedAxis = -1;
            propagateEvent = false;
        }

        return propagateEvent;
    }

    bool ObjectMoveAxisDisplayer::onEscapeKey()
    {
        bool propagateEvent = true;
        if(selectedAxis != -1)
        {
            //TODO cancel

            statusBarController.applyPreviousState();
            selectedAxis = -1;
            propagateEvent = false;
        }
        return propagateEvent;
    }

    void ObjectMoveAxisDisplayer::setSelectedSceneObject(const SceneObject *selectedSceneObject)
    {
        this->selectedSceneObject = selectedSceneObject;
        this->selectedAxis = -1;

        if(selectedSceneObject)
        {
            statusBarController.applyState(StatusBarState::OBJECT_SELECTED);
        }else
        {
            statusBarController.applyPreviousState();
        }
    }

    void ObjectMoveAxisDisplayer::displayAxis()
    {
        cleanCurrentDisplay();

        if (selectedSceneObject)
        {
            GeometryModel *xLine = createAxisModel(selectedSceneObject->getModel(), 0);
            xLine->setColor(1.0, 0.0, 0.0, selectedAxis==0 ? 1.0f : 0.5f);

            GeometryModel *yLine = createAxisModel(selectedSceneObject->getModel(), 1);
            yLine->setColor(0.0, 1.0, 0.0, selectedAxis==1 ? 1.0f : 0.5f);

            GeometryModel *zLine = createAxisModel(selectedSceneObject->getModel(), 2);
            zLine->setColor(0.0, 0.0, 1.0, selectedAxis==2 ? 1.0f : 0.5f);
        }

        for (auto &objectMoveAxisModel : objectMoveAxisModels)
        {
            sceneManager->getActiveRenderer3d()->getGeometryManager()->addGeometry(objectMoveAxisModel);
        }
    }

    void ObjectMoveAxisDisplayer::cleanCurrentDisplay()
    {
        for (auto &objectMoveAxisModel : objectMoveAxisModels)
        {
            sceneManager->getActiveRenderer3d()->getGeometryManager()->removeGeometry(objectMoveAxisModel);
            delete objectMoveAxisModel;
        }

        objectMoveAxisModels.clear();
    }

    GeometryModel *ObjectMoveAxisDisplayer::createAxisModel(Model *model, unsigned int axisIndex)
    {
        Point3<float> startPoint = model->getTransform().getPosition();
        startPoint[axisIndex] -= 500.0f;

        Point3<float> endPoint = model->getTransform().getPosition();
        endPoint[axisIndex] += 500.0f;

        LineSegment3D<float> axeLineSegment(startPoint, endPoint);
        GeometryModel *axisModel = new LinesModel(axeLineSegment, axisIndex==selectedAxis ? 2.0 : 1.0);
        axisModel->setBlendMode(GeometryModel::BlendMode::ONE_MINUS_SRC_ALPHA);
        objectMoveAxisModels.push_back(axisModel);

        return axisModel;
    }
}
