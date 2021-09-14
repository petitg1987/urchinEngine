#include <QCursor>

#include <scene/SceneFreeCamera.h>
#include <StateSaveHelper.h>

namespace urchin {

    SceneFreeCamera::SceneFreeCamera(float horizontalFovAngle, float nearPlane, float farPlane, MouseController& mouseController) :
            FreeCamera(horizontalFovAngle, nearPlane, farPlane),
            mouseXBeforeMove(0.0),
            mouseYBeforeMove(0.0),
            mouseController(mouseController) {

    }

    void SceneFreeCamera::loadCameraState(const std::string& mapFilename) {
        std::map<std::string, std::string> cameraPositionByMap = MapSerializer::deserialize(StateSaveHelper::instance().retrieveState("camera.position", ""));
        auto it = cameraPositionByMap.find(mapFilename);
        if (it != cameraPositionByMap.end()) {
            std::vector<std::string> serializedCameraElements = StringUtil::split(it->second, DATA_DELIMITER);
            assert(serializedCameraElements.size() == 2);

            moveTo(TypeConverter::toPoint3(serializedCameraElements[0]));
            rotate(TypeConverter::toQuaternion(serializedCameraElements[1]));
        }
    }

    void SceneFreeCamera::saveCameraState(const std::string& mapFilename) const {
        std::map<std::string, std::string> cameraPositionByMap = MapSerializer::deserialize(StateSaveHelper::instance().retrieveState("camera.position", ""));

        std::string serializedCameraPosition = std::to_string(getPosition().X) + TypeConverter::FLOAT_DELIMITER + std::to_string(getPosition().Y)
                + TypeConverter::FLOAT_DELIMITER + std::to_string(getPosition().Z);
        std::string serializedCameraOrientation = std::to_string(getOrientation().X) + TypeConverter::FLOAT_DELIMITER + std::to_string(getOrientation().Y)
                + TypeConverter::FLOAT_DELIMITER + std::to_string(getOrientation().Z) + TypeConverter::FLOAT_DELIMITER + std::to_string(getOrientation().W);
        std::string serializedCamera = serializedCameraPosition + DATA_DELIMITER + serializedCameraOrientation;
        cameraPositionByMap[mapFilename] = serializedCamera;

        StateSaveHelper::instance().saveState("camera.position", MapSerializer::serialize(cameraPositionByMap));
    }

    bool SceneFreeCamera::onMouseMove(double mouseX, double mouseY) {
        bool propagateEvent = Camera::onMouseMove(mouseX, mouseY);
        if (isUseMouseToMoveCamera()) {
            mouseController.moveMouse((int)mouseXBeforeMove, (int)mouseYBeforeMove);
            resetPreviousMousePosition(mouseXBeforeMove, mouseYBeforeMove);
        } else {
            mouseXBeforeMove = mouseX;
            mouseYBeforeMove = mouseY;
        }
        return propagateEvent;
    }

}
