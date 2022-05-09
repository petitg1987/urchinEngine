#pragma once

#include <list>
#include <UrchinCommon.h>
#include <UrchinAggregation.h>
#include <Urchin3dEngine.h>

#include <controller/AbstractController.h>

namespace urchin {

    class LightController : public AbstractController {
        public:
            LightController();

            std::list<const LightEntity*> getLightEntities() const;
            void addLightEntity(std::unique_ptr<LightEntity>);
            void removeLightEntity(const LightEntity&);

            void moveLightInFrontOfCamera(const LightEntity&, bool);

            const LightEntity& updateLightGeneralProperties(const LightEntity&, const Point3<float>&, bool);
            const LightEntity& updateOmnidirectionalLightProperties(const LightEntity&, float, const Point3<float>&);
            const LightEntity& updateSunLightProperties(const LightEntity&, const Vector3<float>&);

        private:
            LightEntity& findLightEntity(const LightEntity&);
    };

}
