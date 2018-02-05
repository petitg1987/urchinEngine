#include <stdexcept>

#include "WaterController.h"

namespace urchin
{
    WaterController::WaterController(MapHandler *mapHandler) :
            bIsModified(false),
            mapHandler(mapHandler)
    {

    }

    bool WaterController::isModified() const
    {
        return bIsModified;
    }

    void WaterController::markModified()
    {
        bIsModified = true;
    }

    void WaterController::resetModified()
    {
        bIsModified = false;
    }

    std::list<const SceneWater *> WaterController::getSceneWaters() const
    {
        const std::list<SceneWater *> &sceneWaters = mapHandler->getMap()->getSceneWaters();
        std::list<const SceneWater *> constSceneWaters;
        constSceneWaters.insert(constSceneWaters.begin(), sceneWaters.begin(), sceneWaters.end());

        return constSceneWaters;
    }

    void WaterController::addSceneWater(SceneWater *sceneWater)
    {
        mapHandler->getMap()->addSceneWater(sceneWater);

        markModified();
    }

    void WaterController::removeSceneWater(const SceneWater *constSceneWater)
    {
        SceneWater *sceneWater = findSceneWater(constSceneWater);
        mapHandler->getMap()->removeSceneWater(sceneWater);

        markModified();
    }

    const SceneWater *WaterController::updateSceneWater(const SceneWater *constSceneWater, const Point3<float> &centerPosition, float xSize, float zSize,
                                                        const Vector3<float> &waterColor, const std::string &normalFilename, const std::string &dudvMapFilename,
                                                        float sRepeat, float tRepeat)
    {
        SceneWater *sceneWater = findSceneWater(constSceneWater);
        Water *water = sceneWater->getWater();

        water->setCenterPosition(centerPosition);
        water->setXSize(xSize);
        water->setZSize(zSize);
        water->setWaterColor(waterColor);
        water->setNormalTexture(normalFilename);
        water->setDudvMap(dudvMapFilename);
        water->setSRepeat(sRepeat);
        water->setTRepeat(tRepeat);

        markModified();
        return sceneWater;
    }

    const SceneWater *WaterController::updateSceneWater(const SceneWater *constSceneWater, float density, float gradient)
    {
        SceneWater *sceneWater = findSceneWater(constSceneWater);
        Water *water = sceneWater->getWater();

        water->setDensity(density);
        water->setGradient(gradient);

        markModified();
        return sceneWater;
    }

    SceneWater *WaterController::findSceneWater(const SceneWater *constSceneWater)
    {
        const std::list<SceneWater *> &sceneWaters = mapHandler->getMap()->getSceneWaters();
        auto it = std::find(sceneWaters.begin(), sceneWaters.end(), constSceneWater);

        if(it!=sceneWaters.end())
        {
            return *it;
        }

        throw std::invalid_argument("Impossible to find scene water: " + constSceneWater->getName());
    }
}
