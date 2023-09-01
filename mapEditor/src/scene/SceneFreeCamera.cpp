#include <QCursor>

#include <scene/SceneFreeCamera.h>
#include <StateSaveHelper.h>

namespace urchin {

    SceneFreeCamera::SceneFreeCamera(float horizontalFovAngle, float nearPlane, float farPlane, MouseController& mouseController) :
            FreeCamera(horizontalFovAngle, nearPlane, farPlane),
            disableNextMouseEvent(false),
            mouseController(mouseController) {

    }

    void SceneFreeCamera::loadCameraState(const std::string& mapFilename) {
        std::map<std::string, std::string, std::less<>> cameraPositionByMap = MapSerializer::deserialize(StateSaveHelper::instance().retrieveState("camera.position", ""));
        auto it = cameraPositionByMap.find(mapFilename);
        if (it != cameraPositionByMap.end()) {
            std::vector<std::string> serializedCameraElements = StringUtil::split(it->second, DATA_DELIMITER);
            assert(serializedCameraElements.size() == 2);

            moveTo(TypeConverter::toPoint3(serializedCameraElements[0]));
            rotate(TypeConverter::toQuaternion(serializedCameraElements[1]));
        }
    }

    void SceneFreeCamera::saveCameraState(const std::string& mapFilename) const {
        std::map<std::string, std::string, std::less<>> cameraPositionByMap = MapSerializer::deserialize(StateSaveHelper::instance().retrieveState("camera.position", ""));

        std::string serializedCameraPosition = TypeConverter::toString(getPosition().X) + TypeConverter::NUMBER_DELIMITER + TypeConverter::toString(getPosition().Y)
                + TypeConverter::NUMBER_DELIMITER + TypeConverter::toString(getPosition().Z);
        std::string serializedCameraOrientation = TypeConverter::toString(getOrientation().X) + TypeConverter::NUMBER_DELIMITER + TypeConverter::toString(getOrientation().Y)
                + TypeConverter::NUMBER_DELIMITER + TypeConverter::toString(getOrientation().Z) + TypeConverter::NUMBER_DELIMITER + TypeConverter::toString(getOrientation().W);
        std::string serializedCamera = serializedCameraPosition + DATA_DELIMITER + serializedCameraOrientation;
        cameraPositionByMap[mapFilename] = serializedCamera;

        StateSaveHelper::instance().saveState("camera.position", MapSerializer::serialize(cameraPositionByMap));
    }

    bool SceneFreeCamera::onMouseMove(double deltaMouseX, double deltaMouseY) {
        bool propagateEvent = true;
        if (!disableNextMouseEvent) {
            propagateEvent = Camera::onMouseMove(deltaMouseX, deltaMouseY);
            if (Camera::isUseMouseToMoveCamera()) {
                mouseController.moveMouse((double) getSceneWidth() / 2.0, (double) getSceneHeight() / 2.0);
                disableNextMouseEvent = true;
            }
        } else {
            disableNextMouseEvent = false;
        }
        return propagateEvent;
    }

}
