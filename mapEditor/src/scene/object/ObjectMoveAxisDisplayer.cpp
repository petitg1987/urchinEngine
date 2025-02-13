#include <scene/object/ObjectMoveAxisDisplayer.h>

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
        xLine.setColor(Vector3(1.0f, 0.0f, 0.0f));

        GeometryModel& yLine = createAxisModel(position, selectedAxis, 1);
        yLine.setColor(Vector3(0.0f, 1.0f, 0.0f));

        GeometryModel& zLine = createAxisModel(position, selectedAxis, 2);
        zLine.setColor(Vector3(0.0f, 0.0f, 1.0f));

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
        constexpr float INACTIVE_WIDTH = 0.01f;
        constexpr float ACTIVE_WIDTH = 0.02f;

        Point3<float> startPoint = position;
        Point3<float> endPoint = position;
        endPoint[axisIndex] += 0.4f;

        Vector3<float> axeVector = startPoint.vector(endPoint);
        Point3<float> axeCenter = startPoint.translate((axeVector * 0.5f) + (axeVector.normalize() * (INACTIVE_WIDTH + 0.001f)));
        Quaternion<float> axeOrientation = Quaternion<float>::rotationFromTo(Vector3(1.0f, 0.0f, 0.0f), axeVector.normalize()).normalize();
        float radius = (axisIndex == selectedAxis) ? ACTIVE_WIDTH : INACTIVE_WIDTH;
        auto axisModel = std::make_shared<CapsuleModel>(Capsule(radius, axeVector.length(), CapsuleShape<float>::CAPSULE_X, axeCenter, axeOrientation), 15, 10);
        axisModel->setAlwaysVisible(true);
        return *objectMoveAxisModels.emplace_back(std::move(axisModel));
    }
}
