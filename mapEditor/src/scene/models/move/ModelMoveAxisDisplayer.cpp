#include <scene/models/move/ModelMoveAxisDisplayer.h>

namespace urchin {
    ModelMoveAxisDisplayer::ModelMoveAxisDisplayer(Scene& scene) :
            scene(scene) {

    }

    ModelMoveAxisDisplayer::~ModelMoveAxisDisplayer() {
        cleanCurrentDisplay();
    }

    void ModelMoveAxisDisplayer::displayAxis(const Point3<float>& position, unsigned int selectedAxis) {
        cleanCurrentDisplay();

        GeometryModel& xLine = createAxisModel(position, selectedAxis, 0);
        xLine.setColor(1.0f, 0.0f, 0.0f);

        GeometryModel& yLine = createAxisModel(position, selectedAxis, 1);
        yLine.setColor(0.0f, 1.0f, 0.0f);

        GeometryModel& zLine = createAxisModel(position, selectedAxis, 2);
        zLine.setColor(0.0f, 0.0f, 1.0f);

        for (const auto& modelMoveAxisModel : modelMoveAxisModels) {
            scene.getActiveRenderer3d()->getGeometryContainer().addGeometry(modelMoveAxisModel);
        }
    }

    void ModelMoveAxisDisplayer::cleanCurrentDisplay() {
        for (const auto& modelMoveAxisModel : modelMoveAxisModels) {
            scene.getActiveRenderer3d()->getGeometryContainer().removeGeometry(*modelMoveAxisModel);
        }
        modelMoveAxisModels.clear();
    }

    GeometryModel& ModelMoveAxisDisplayer::createAxisModel(const Point3<float>& position, unsigned int selectedAxis, std::size_t axisIndex) {
        Point3<float> startPoint = position;
        Point3<float> endPoint = position;
        endPoint[axisIndex] += 0.4f;

        Vector3<float> axeVector = startPoint.vector(endPoint);
        Point3<float> axeCenter = startPoint.translate(axeVector * 0.5f);
        Quaternion<float> axeOrientation = Quaternion<float>::rotationFromTo(Vector3<float>(1.0f, 0.0f, 0.0f), axeVector.normalize()).normalize();
        float radius = (axisIndex == selectedAxis) ? 0.03f : 0.01f;
        auto axisModel = std::make_shared<CylinderModel>(Cylinder<float>(radius, axeVector.length(), CylinderShape<float>::CYLINDER_X, axeCenter, axeOrientation), 10);
        axisModel->setPolygonMode(PolygonMode::FILL);
        axisModel->setAlwaysVisible(true);
        modelMoveAxisModels.push_back(std::move(axisModel));

        return *modelMoveAxisModels.back();
    }
}
