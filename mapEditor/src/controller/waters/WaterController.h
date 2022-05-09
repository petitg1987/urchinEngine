#pragma once

#include <list>
#include <UrchinCommon.h>
#include <UrchinAggregation.h>
#include <Urchin3dEngine.h>

#include <controller/AbstractController.h>

namespace urchin {

    class WaterController : public AbstractController {
        public:
            WaterController();

            std::list<const WaterEntity*> getWaterEntities() const;
            void addWaterEntity(std::unique_ptr<WaterEntity>);
            void removeWaterEntity(const WaterEntity&);

            void moveWaterInFrontOfCamera(const WaterEntity&, bool);

            const WaterEntity& updateWaterGeneral(const WaterEntity&, const Point3<float>&, float, float);
            const WaterEntity& updateWaterSurface(const WaterEntity&, const Vector3<float>&, std::string, std::string, float, float, float, float);
            const WaterEntity& updateWaterUnderWater(const WaterEntity&, float, float);

        private:
            WaterEntity& findWaterEntity(const WaterEntity&);
    };

}
