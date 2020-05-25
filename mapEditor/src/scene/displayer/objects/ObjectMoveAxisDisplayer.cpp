#include "ObjectMoveAxisDisplayer.h"

namespace urchin
{
    ObjectMoveAxisDisplayer::ObjectMoveAxisDisplayer(SceneManager *sceneManager) :
            sceneManager(sceneManager),
            displayedObject(nullptr),
            selectedAxis(-1),
            oldMouseX(-1),
            oldMouseY(-1)
    {

    }

    ObjectMoveAxisDisplayer::~ObjectMoveAxisDisplayer()
    {
        cleanCurrentDisplay();
    }

    void ObjectMoveAxisDisplayer::startMove(unsigned int axisIndex)
    {
        this->selectedAxis = axisIndex;
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
        Matrix4<float> cameraViewMatrix = sceneManager->getActiveRenderer3d()->getCamera()->getViewMatrix();
        for(unsigned int i=0; i<16; ++i)
        {
            if(this->oldCameraViewMatrix(i) != cameraViewMatrix(i))
            {
                return true;
            }
        }
        return false;
    }

    void ObjectMoveAxisDisplayer::moveObject(const Point2<float> &oldMousePosition, const Point2<float> &newMousePosition)
    { //TODO impl
        Transform<float> t = displayedObject->getModel()->getTransform();
        updateObjectPosition(Point3<float>(t.getPosition().X + 0.1, t.getPosition().Y, t.getPosition().Z));
    }

    void ObjectMoveAxisDisplayer::updateObjectPosition(const Point3<float> &newPosition)
    { //TODO inform mapEditor of change on object to ask to save (see ObjectController::updateSceneObjectTransform)
        Transform<float> transform = displayedObject->getModel()->getTransform();
        transform.setPosition(newPosition);

        displayedObject->getModel()->setTransform(transform);
        if(displayedObject->getRigidBody())
        {
            displayedObject->getRigidBody()->setTransform(transform);
        }
    }

    bool ObjectMoveAxisDisplayer::onMouseLeftButton()
    {
        bool propagateEvent = true;

        if(selectedAxis != -1)
        {
            //TODO

            propagateEvent = false;
            selectedAxis = -1;
        }

        return propagateEvent;
    }

    bool ObjectMoveAxisDisplayer::onEscapeKey()
    {
        bool propagateEvent = true;
        if(selectedAxis != -1)
        {
            selectedAxis = -1;
            propagateEvent = false;
        }
        return propagateEvent;
    }

    void ObjectMoveAxisDisplayer::displayAxisFor(const SceneObject *sceneObject)
    {
        if(displayedObject != sceneObject)
        {
            this->displayedObject = sceneObject;
            selectedAxis = -1;
        }

        cleanCurrentDisplay();

        if (sceneObject)
        {
            GeometryModel *xLine = createAxisModel(sceneObject->getModel(), 0);
            xLine->setColor(1.0, 0.0, 0.0, selectedAxis==0 ? 1.0f : 0.5f);

            GeometryModel *yLine = createAxisModel(sceneObject->getModel(), 1);
            yLine->setColor(0.0, 1.0, 0.0, selectedAxis==1 ? 1.0f : 0.5f);

            GeometryModel *zLine = createAxisModel(sceneObject->getModel(), 2);
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
        startPoint[axisIndex] -= 1000.0f;

        Point3<float> endPoint = model->getTransform().getPosition();
        endPoint[axisIndex] += 1000.0f;

        LineSegment3D<float> axeLineSegment(startPoint, endPoint);
        GeometryModel *axisModel = new LinesModel(axeLineSegment, axisIndex==selectedAxis ? 2.0 : 1.0);
        axisModel->setBlendMode(GeometryModel::BlendMode::ONE_MINUS_SRC_ALPHA);
        objectMoveAxisModels.push_back(axisModel);

        return axisModel;
    }
}
