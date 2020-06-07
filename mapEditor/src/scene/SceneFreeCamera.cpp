#include <QCursor>

#include "SceneFreeCamera.h"
#include "StateSaveHelper.h"

namespace urchin
{

    //static
    const char SceneFreeCamera::DATA_DELIMITER = '@';

	SceneFreeCamera::SceneFreeCamera(float angle, float nearPlane, float farPlane, const MouseController &mouseController) :
		FreeCamera(angle, nearPlane, farPlane),
        mouseController(mouseController)
	{

	}

	void SceneFreeCamera::moveMouse(unsigned int x, unsigned int y)
	{
        mouseController.moveMouse(x, y);
	}

    void SceneFreeCamera::loadCameraState(const std::string &mapFilename)
    {
        std::map<std::string, std::string> cameraPositionByMap = MapUtil::deserialize(StateSaveHelper::instance()->retrieveState("camera.position", ""));
        auto it = cameraPositionByMap.find(mapFilename);
        if(it != cameraPositionByMap.end())
        {
            std::vector<std::string> serializedCameraElements;
            StringUtil::split(it->second, DATA_DELIMITER, serializedCameraElements);
            assert(serializedCameraElements.size() == 2);

            moveTo(Converter::toPoint3(serializedCameraElements[0]));
            lookAt(Converter::toVector3(serializedCameraElements[1]));
        }
    }

    void SceneFreeCamera::saveCameraState(const std::string &mapFilename) const
    {
        std::map<std::string, std::string> cameraPositionByMap = MapUtil::deserialize(StateSaveHelper::instance()->retrieveState("camera.position", ""));

        std::string serializedCameraPosition = std::to_string(getPosition().X) + Converter::FLOAT_DELIMITER + std::to_string(getPosition().Y)
                + Converter::FLOAT_DELIMITER + std::to_string(getPosition().Z);
        std::string serializedCameraView = std::to_string(getView().X) + Converter::FLOAT_DELIMITER + std::to_string(getView().Y)
                + Converter::FLOAT_DELIMITER + std::to_string(getView().Z);
        std::string serializedCamera = serializedCameraPosition + DATA_DELIMITER + serializedCameraView;
        cameraPositionByMap[mapFilename] = serializedCamera;

        StateSaveHelper::instance()->saveState("camera.position", MapUtil::serialize(cameraPositionByMap));
    }

}
