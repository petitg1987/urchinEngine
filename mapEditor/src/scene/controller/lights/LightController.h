#ifndef ENGINE_MAPEDITOR_LIGHTCONTROLLER_H
#define ENGINE_MAPEDITOR_LIGHTCONTROLLER_H

#include <list>
#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "Urchin3dEngine.h"

namespace urchin
{

	class LightController
	{
		public:
			explicit LightController(MapHandler *);

			bool isModified() const;
			void resetModified();

			std::list<const SceneLight *> getSceneLights() const;
			void addSceneLight(SceneLight *);
			void removeSceneLight(const SceneLight *);

			const SceneLight *updateSceneLightGeneralProperties(const SceneLight *, const Point3<float> &, bool);
			const SceneLight *updateSceneOmnidirectionalLightProperties(const SceneLight *, float, const Point3<float> &);
			const SceneLight *updateSceneSunLightProperties(const SceneLight *, const Vector3<float> &);

		private:
			void markModified();
			SceneLight *findSceneLight(const SceneLight *);

			bool bIsModified;
			MapHandler *mapHandler;
	};

}

#endif
