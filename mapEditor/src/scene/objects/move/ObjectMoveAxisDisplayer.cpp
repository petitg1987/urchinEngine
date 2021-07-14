#include <scene/objects/move/ObjectMoveAxisDisplayer.h>

namespace urchin {
    ObjectMoveAxisDisplayer::ObjectMoveAxisDisplayer(SceneManager& sceneManager) :
            sceneManager(sceneManager) {

    }

    ObjectMoveAxisDisplayer::~ObjectMoveAxisDisplayer() {
        cleanCurrentDisplay();
    }

    void ObjectMoveAxisDisplayer::displayAxis(const Point3<float>& position, unsigned int selectedAxis) {
        cleanCurrentDisplay();

        GeometryModel& xLine = createAxisModel(position, selectedAxis, 0);
        xLine.setColor(1.0f, 0.0f, 0.0f);

        GeometryModel& yLine = createAxisModel(position, selectedAxis, 1);
        yLine.setColor(0.0f, 1.0f, 0.0f);

        GeometryModel& zLine = createAxisModel(position, selectedAxis, 2);
        zLine.setColor(0.0f, 0.0f, 1.0f);

        for (const auto& objectMoveAxisModel : objectMoveAxisModels) {
            sceneManager.getActiveRenderer3d()->getGeometryManager().addGeometry(objectMoveAxisModel);
        }
    }

    void ObjectMoveAxisDisplayer::cleanCurrentDisplay() {
        for (const auto& objectMoveAxisModel : objectMoveAxisModels) {
            sceneManager.getActiveRenderer3d()->getGeometryManager().removeGeometry(*objectMoveAxisModel);
        }
        objectMoveAxisModels.clear();
    }

    GeometryModel& ObjectMoveAxisDisplayer::createAxisModel(const Point3<float>& position, unsigned int selectedAxis, std::size_t axisIndex) {
        Point3<float> startPoint = position;
        Point3<float> endPoint = position;
        endPoint[axisIndex] += 0.4f;

        LineSegment3D<float> axeLineSegment(startPoint, endPoint);
        auto axisModel = std::make_unique<LinesModel>(axeLineSegment);
        axisModel->setLineWidth(axisIndex == selectedAxis ? 3.0f : 1.0f);
        axisModel->setAlwaysVisible(true);
        objectMoveAxisModels.push_back(std::move(axisModel));

        return *objectMoveAxisModels.back();
    }
}
