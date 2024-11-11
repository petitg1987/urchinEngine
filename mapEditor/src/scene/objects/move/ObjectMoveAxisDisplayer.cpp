#include <scene/objects/move/ObjectMoveAxisDisplayer.h>

namespace urchin {
    ObjectMoveAxisDisplayer::ObjectMoveAxisDisplayer(Scene& scene) :
            scene(scene) {

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
            scene.getActiveRenderer3d()->getGeometryContainer().addGeometry(objectMoveAxisModel);
        }
    }

    void ObjectMoveAxisDisplayer::cleanCurrentDisplay() {
        for (const auto& objectMoveAxisModel : objectMoveAxisModels) {
            scene.getActiveRenderer3d()->getGeometryContainer().removeGeometry(*objectMoveAxisModel);
        }
        objectMoveAxisModels.clear();
    }

    GeometryModel& ObjectMoveAxisDisplayer::createAxisModel(const Point3<float>& position, unsigned int selectedAxis, std::size_t axisIndex) {
        Point3<float> startPoint = position;
        Point3<float> endPoint = position;
        endPoint[axisIndex] += 0.4f;

        Vector3<float> axeVector = startPoint.vector(endPoint);
        Point3<float> axeCenter = startPoint.translate(axeVector * 0.5f);
        Quaternion<float> axeOrientation = Quaternion<float>::rotationFromTo(Vector3(1.0f, 0.0f, 0.0f), axeVector.normalize()).normalize();
        float radius = (axisIndex == selectedAxis) ? 0.03f : 0.01f;
        auto axisModel = std::make_shared<CylinderModel>(Cylinder(radius, axeVector.length(), CylinderShape<float>::CYLINDER_X, axeCenter, axeOrientation), 10);
        axisModel->setAlwaysVisible(true);
        return *objectMoveAxisModels.emplace_back(std::move(axisModel));
    }
}
