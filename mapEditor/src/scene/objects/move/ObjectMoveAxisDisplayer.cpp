#include "ObjectMoveAxisDisplayer.h"

namespace urchin {
    ObjectMoveAxisDisplayer::ObjectMoveAxisDisplayer(SceneManager* sceneManager) :
            sceneManager(sceneManager) {

    }

    ObjectMoveAxisDisplayer::~ObjectMoveAxisDisplayer() {
        cleanCurrentDisplay();
    }

    void ObjectMoveAxisDisplayer::displayAxis(const Point3<float>& position, unsigned int selectedAxis) {
        cleanCurrentDisplay();

        GeometryModel* xLine = createAxisModel(position, selectedAxis, 0);
        xLine->setColor(1.0, 0.0, 0.0, selectedAxis == 0 ? 1.0f : 0.5f);

        GeometryModel* yLine = createAxisModel(position, selectedAxis, 1);
        yLine->setColor(0.0, 1.0, 0.0, selectedAxis == 1 ? 1.0f : 0.5f);

        GeometryModel* zLine = createAxisModel(position, selectedAxis, 2);
        zLine->setColor(0.0, 0.0, 1.0, selectedAxis == 2 ? 1.0f : 0.5f);

        for (auto& objectMoveAxisModel : objectMoveAxisModels) {
            sceneManager->getActiveRenderer3d()->getGeometryManager()->addGeometry(objectMoveAxisModel);
        }
    }

    void ObjectMoveAxisDisplayer::cleanCurrentDisplay() {
        for (auto& objectMoveAxisModel : objectMoveAxisModels) {
            sceneManager->getActiveRenderer3d()->getGeometryManager()->removeGeometry(objectMoveAxisModel);
            delete objectMoveAxisModel;
        }

        objectMoveAxisModels.clear();
    }

    GeometryModel* ObjectMoveAxisDisplayer::createAxisModel(const Point3<float>& position, unsigned int selectedAxis, std::size_t axisIndex) {
        Point3<float> startPoint = position;
        startPoint[axisIndex] -= 500.0f;

        Point3<float> endPoint = position;
        endPoint[axisIndex] += 500.0f;

        LineSegment3D<float> axeLineSegment(startPoint, endPoint);
        GeometryModel* axisModel = new LinesModel(axeLineSegment);
        axisModel->setLineWidth(axisIndex == selectedAxis ? 2.0f : 1.0f);
        axisModel->enableTransparency();
        objectMoveAxisModels.push_back(axisModel);

        return axisModel;
    }
}
