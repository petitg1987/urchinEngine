#include <stdexcept>

#include "WaterController.h"

namespace urchin
{
    WaterController::WaterController(MapHandler *mapHandler) :
            AbstractController(mapHandler)
    {

    }

    std::list<const SceneWater *> WaterController::getSceneWaters() const
    {
        const std::list<SceneWater *> &sceneWaters = getMapHandler()->getMap()->getSceneWaters();
        std::list<const SceneWater *> constSceneWaters;
        constSceneWaters.insert(constSceneWaters.begin(), sceneWaters.begin(), sceneWaters.end());

        return constSceneWaters;
    }

    void WaterController::addSceneWater(SceneWater *sceneWater)
    {
        getMapHandler()->getMap()->addSceneWater(sceneWater);

        markModified();
    }

    void WaterController::removeSceneWater(const SceneWater *constSceneWater)
    {
        SceneWater *sceneWater = findSceneWater(constSceneWater);
        getMapHandler()->getMap()->removeSceneWater(sceneWater);

        markModified();
    }

    const SceneWater *WaterController::updateSceneWaterGeneral(const SceneWater *constSceneWater, const Point3<float> &centerPosition, float xSize, float zSize)
    {
        SceneWater *sceneWater = findSceneWater(constSceneWater);
        Water *water = sceneWater->getWater();

        water->setCenterPosition(centerPosition);
        water->setXSize(xSize);
        water->setZSize(zSize);

        markModified();
        return sceneWater;
    }

    const SceneWater *WaterController::updateSceneWaterSurface(const SceneWater *constSceneWater, const Vector3<float> &waterColor, const std::string &normalFilename,
                                                               const std::string &dudvMapFilename, float waveSpeed, float waveStrength, float sRepeat, float tRepeat)
    {
        SceneWater *sceneWater = findSceneWater(constSceneWater);
        Water *water = sceneWater->getWater();

        water->setWaterColor(waterColor);
        water->setNormalTexture(normalFilename);
        water->setDudvMap(dudvMapFilename);
        water->setWaveSpeed(waveSpeed);
        water->setWaveStrength(waveStrength);
        water->setSRepeat(sRepeat);
        water->setTRepeat(tRepeat);

        markModified();
        return sceneWater;
    }

    const SceneWater *WaterController::updateSceneWaterUnderWater(const SceneWater *constSceneWater, float density, float gradient)
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
        const std::list<SceneWater *> &sceneWaters = getMapHandler()->getMap()->getSceneWaters();
        auto it = std::find(sceneWaters.begin(), sceneWaters.end(), constSceneWater);

        if(it!=sceneWaters.end())
        {
            return *it;
        }

        throw std::invalid_argument("Impossible to find scene water: " + constSceneWater->getName());
    }
}
