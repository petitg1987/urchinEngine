#include "ObjectMoveAxisDisplayer.h"

namespace urchin
{
    ObjectMoveAxisDisplayer::ObjectMoveAxisDisplayer(SceneManager *sceneManager) :
            sceneManager(sceneManager)
    {

    }

    ObjectMoveAxisDisplayer::~ObjectMoveAxisDisplayer()
    {
        cleanCurrentDisplay();
    }


    void ObjectMoveAxisDisplayer::displayAxisFor(const SceneObject *sceneObject)
    {
        cleanCurrentDisplay();

        if (sceneObject)
        {
            GeometryModel *xLine = createAxisModel(sceneObject->getModel(), 0);
            xLine->setColor(1.0, 0.0, 0.0, 0.4);

            GeometryModel *yLine = createAxisModel(sceneObject->getModel(), 1);
            yLine->setColor(0.0, 1.0, 0.0, 0.4);

            GeometryModel *zLine = createAxisModel(sceneObject->getModel(), 2);
            zLine->setColor(0.0, 0.0, 1.0, 0.4);
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
        GeometryModel *axisModel = new LinesModel(axeLineSegment, 2.0f);
        axisModel->setBlendMode(GeometryModel::BlendMode::ONE_MINUS_SRC_ALPHA);
        objectMoveAxisModels.push_back(axisModel);

        return axisModel;
    }
}
