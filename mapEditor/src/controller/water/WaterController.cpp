#include <stdexcept>
#include <utility>

#include "controller/water/WaterController.h"
#include "controller/EntityControllerUtil.h"

namespace urchin {

    std::list<const WaterEntity*> WaterController::getWaterEntities() const {
        const auto& waterEntities = getMap().getWaterEntities();
        std::list<const WaterEntity*> constWaterEntities;
        for (auto& waterEntity : waterEntities) {
            constWaterEntities.emplace_back(waterEntity.get());
        }

        return constWaterEntities;
    }

    void WaterController::addWaterEntity(std::unique_ptr<WaterEntity> waterEntity) {
        getMap().addWaterEntity(std::move(waterEntity));

        markModified();
    }

    void WaterController::removeWaterEntity(const WaterEntity& constWaterEntity) {
        WaterEntity& waterEntity = findWaterEntity(constWaterEntity);
        getMap().removeWaterEntity(waterEntity);

        markModified();
    }

    void WaterController::moveWaterInFrontOfCamera(const WaterEntity& constWaterEntity, bool isClonedEntity) {
        Point3<float> currentPosition = constWaterEntity.getWater()->getCenterPosition();
        Point3<float> newPosition = EntityControllerUtil::determineNewPosition(currentPosition, isClonedEntity, getMap().getRenderer3d()->getCamera());
        constWaterEntity.getWater()->setCenterPosition(newPosition);

        markModified();
    }

    const WaterEntity& WaterController::updateWaterGeneral(const WaterEntity& constWaterEntity, const Point3<float>& centerPosition, float xSize, float zSize) {
        const WaterEntity& waterEntity = findWaterEntity(constWaterEntity);
        Water* water = waterEntity.getWater();

        water->setCenterPosition(centerPosition);
        water->setXSize(xSize);
        water->setZSize(zSize);

        markModified();
        return waterEntity;
    }

    const WaterEntity& WaterController::updateWaterSurface(const WaterEntity& constWaterEntity, const Vector3<float>& waterColor, std::string normalFilename,
                                                           std::string dudvMapFilename, float waveSpeed, float waveStrength, float sRepeat, float tRepeat) {
        const WaterEntity& waterEntity = findWaterEntity(constWaterEntity);
        Water* water = waterEntity.getWater();

        water->setWaterColor(waterColor);
        water->setNormalTexture(std::move(normalFilename));
        water->setDudvMap(std::move(dudvMapFilename));
        water->setWaveSpeed(waveSpeed);
        water->setWaveStrength(waveStrength);
        water->setSRepeat(sRepeat);
        water->setTRepeat(tRepeat);

        markModified();
        return waterEntity;
    }

    const WaterEntity& WaterController::updateWaterUnderWater(const WaterEntity& constWaterEntity, float density, float gradient) {
        const WaterEntity& waterEntity = findWaterEntity(constWaterEntity);
        Water* water = waterEntity.getWater();

        water->setDensity(density);
        water->setGradient(gradient);

        markModified();
        return waterEntity;
    }

    WaterEntity& WaterController::findWaterEntity(const WaterEntity& constWaterEntity) const {
        const auto& watersEntities = getMap().getWaterEntities();
        auto it = std::ranges::find_if(watersEntities, [&constWaterEntity](const auto& o){return o.get() == &constWaterEntity;});

        if (it != watersEntities.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find water entity: " + constWaterEntity.getName());
    }
}
