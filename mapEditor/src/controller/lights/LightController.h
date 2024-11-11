#pragma once

#include <list>
#include <UrchinCommon.h>
#include <UrchinAggregation.h>

#include <controller/AbstractController.h>

namespace urchin {

    class LightController final : public AbstractController {
        public:
            std::list<const LightEntity*> getLightEntities() const;
            void addLightEntity(std::unique_ptr<LightEntity>);
            void removeLightEntity(const LightEntity&);

            void moveLightInFrontOfCamera(const LightEntity&, bool);

            const LightEntity& updateLightGeneralProperties(const LightEntity&, const Point3<float>&, bool, bool);
            const LightEntity& updateSunLightProperties(const LightEntity&, const Vector3<float>&);
            const LightEntity& updateOmnidirectionalLightProperties(const LightEntity&, float, const Point3<float>&);
            const LightEntity& updateSpotLightProperties(const LightEntity&, float, const Point3<float>&, const Vector3<float>&, float, float);

        private:
            LightEntity& findLightEntity(const LightEntity&) const;
    };

}
