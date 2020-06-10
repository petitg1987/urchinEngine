#ifndef URCHINENGINE_LIGHTCONTROLLER_H
#define URCHINENGINE_LIGHTCONTROLLER_H

#include <list>
#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "Urchin3dEngine.h"

#include "controller/AbstractController.h"

namespace urchin
{

	class LightController : public AbstractController
	{
		public:
			LightController();

			std::list<const SceneLight *> getSceneLights() const;
			void addSceneLight(SceneLight *);
			void removeSceneLight(const SceneLight *);

			const SceneLight *updateSceneLightGeneralProperties(const SceneLight *, const Point3<float> &, bool);
			const SceneLight *updateSceneOmnidirectionalLightProperties(const SceneLight *, float, const Point3<float> &);
			const SceneLight *updateSceneSunLightProperties(const SceneLight *, const Vector3<float> &);

		private:
			SceneLight *findSceneLight(const SceneLight *);
	};

}

#endif
