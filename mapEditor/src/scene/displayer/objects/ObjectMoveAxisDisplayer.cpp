#include "ObjectMoveAxisDisplayer.h"

namespace urchin
{
    ObjectMoveAxisDisplayer::ObjectMoveAxisDisplayer(SceneManager *sceneManager) :
            sceneManager(sceneManager),
            displayedObject(nullptr),
            selectedAxis(-1)
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
        //TODO

        bool propagateEvent = selectedAxis == -1;
        return propagateEvent;
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

    bool ObjectMoveAxisDisplayer::onMouseRightButton()
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
