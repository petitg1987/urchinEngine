#ifndef ENGINE_LIGHTMANAGER_H
#define ENGINE_LIGHTMANAGER_H

#include <vector>
#include "UrchinCommon.h"

#include "Light.h"
#include "scene/renderer3d/octree/OctreeManager.h"

namespace urchin
{

	class LightManager : public Observable
	{
		public:
			LightManager();
			virtual ~LightManager();

			enum NotificationType
			{
				ADD_LIGHT, //A light has been added
				REMOVE_LIGHT, //A light has been removed
			};

			void initialize(unsigned int);
			OctreeManager<Light> *getLightOctreeManager() const;
			Light *getLastUpdatedLight();

			unsigned int getMaxLights() const;
			const std::vector<Light *> &getVisibleLights() const;
			void addLight(Light *const);
			void removeLight(Light *const);

			void setGlobalAmbientColor(const Point4<float> &);
			const Point4<float> &getGlobalAmbientColor() const;

			void updateLights(const Frustum<float> &);
			void loadLights();
			void postUpdateLights();

			#ifdef _DEBUG
				void drawLightOctree(const Matrix4<float> &, const Matrix4<float> &) const;
			#endif

		private:
			void onLightEvent(Light *const, NotificationType);

			bool isInitialized;

			//lights container
			std::vector<Light *> parallelBeamsLights; //sun lights
			OctreeManager<Light> *lightOctreeManager; //others lights
			std::vector<Light *> visibleLights;

			Light *lastUpdatedLight;

			const unsigned int maxLights; //maximum of lights authorized to affect the scene in the same time
			struct LightInfo
			{
				int isExistLoc;
				int produceShadowLoc;
				int hasParallelBeamsLoc;
				int positionOrDirectionLoc;

				int exponentialAttLoc;
				int lightAmbientLoc;
			};
			LightInfo *lightsInfo;

			int globalAmbientColorLoc;
			Point4<float> globalAmbientColor;
	};

}

#endif
