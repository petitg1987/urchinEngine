#include <QCursor>

#include <scene/SceneFreeCamera.h>
#include <StateSaveHelper.h>

namespace urchin {

    //static
    const char SceneFreeCamera::DATA_DELIMITER = '@';

    SceneFreeCamera::SceneFreeCamera(float angle, float nearPlane, float farPlane, const std::unique_ptr<MouseController>& mouseController) :
            FreeCamera(angle, nearPlane, farPlane),
            mouseXBeforeMove(0),
            mouseYBeforeMove(0),
            mouseController(mouseController) {

    }

    void SceneFreeCamera::loadCameraState(const std::string& mapFilename) {
        std::map<std::string, std::string> cameraPositionByMap = MapSerializer::deserialize(StateSaveHelper::instance()->retrieveState("camera.position", ""));
        auto it = cameraPositionByMap.find(mapFilename);
        if (it != cameraPositionByMap.end()) {
            std::vector<std::string> serializedCameraElements;
            StringUtil::split(it->second, DATA_DELIMITER, serializedCameraElements);
            assert(serializedCameraElements.size() == 2);

            moveTo(TypeConverter::toPoint3(serializedCameraElements[0]));
            lookAt(TypeConverter::toVector3(serializedCameraElements[1]));
        }
    }

    void SceneFreeCamera::saveCameraState(const std::string& mapFilename) const {
        std::map<std::string, std::string> cameraPositionByMap = MapSerializer::deserialize(StateSaveHelper::instance()->retrieveState("camera.position", ""));

        std::string serializedCameraPosition = std::to_string(getPosition().X) + TypeConverter::FLOAT_DELIMITER + std::to_string(getPosition().Y)
                + TypeConverter::FLOAT_DELIMITER + std::to_string(getPosition().Z);
        std::string serializedCameraView = std::to_string(getView().X) + TypeConverter::FLOAT_DELIMITER + std::to_string(getView().Y)
                + TypeConverter::FLOAT_DELIMITER + std::to_string(getView().Z);
        std::string serializedCamera = serializedCameraPosition + DATA_DELIMITER + serializedCameraView;
        cameraPositionByMap[mapFilename] = serializedCamera;

        StateSaveHelper::instance()->saveState("camera.position", MapSerializer::serialize(cameraPositionByMap));
    }

    bool SceneFreeCamera::onMouseMove(double mouseX, double mouseY) {
        bool propagateEvent = Camera::onMouseMove(mouseX, mouseY);
        if (isUseMouseToMoveCamera()) {
            mouseController->moveMouse((int)mouseXBeforeMove, (int)mouseYBeforeMove);
            resetPreviousMousePosition(mouseXBeforeMove, mouseYBeforeMove);
        } else {
            mouseXBeforeMove = mouseX;
            mouseYBeforeMove = mouseY;
        }
        return propagateEvent;
    }

}
