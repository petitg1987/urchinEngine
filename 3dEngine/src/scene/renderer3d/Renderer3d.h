#ifndef ENGINE_RENDERER3D_H
#define ENGINE_RENDERER3D_H

#include <string>
#include <memory>
#include "UrchinCommon.h"

#include "scene/Renderer.h"
#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/camera/FpsCamera.h"
#include "scene/renderer3d/camera/FreeCamera.h"
#include "scene/renderer3d/model/displayer/ModelDisplayer.h"
#include "scene/renderer3d/antialiasing/AntiAliasingManager.h"
#include "scene/renderer3d/octree/OctreeManager.h"
#include "scene/renderer3d/octree/Octreeable.h"
#include "scene/renderer3d/shadow/ShadowManager.h"
#include "scene/renderer3d/ambientocclusion/AmbientOcclusionManager.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/skybox/Skybox.h"
#include "utils/display/geometry/GeometryDisplayer.h"
#include "utils/display/geometry/GeometryModel.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

	class Renderer3d : public Renderer, public Observer
	{
		public:
			Renderer3d();
			virtual ~Renderer3d();

			//scene properties
			void onResize(unsigned int, unsigned int);
			void notify(Observable *, int);

			//managers
			OctreeManager<Model> *getModelOctreeManager() const;

			LightManager *getLightManager() const;
			void activateLighting(bool);

			ShadowManager *getShadowManager() const;
			void activateShadow(bool);

			AmbientOcclusionManager *getAmbientOcclusionManager() const;
			void activateAmbientOcclusion(bool);

			AntiAliasingManager *getAntiAliasingManager() const;
			void activateAntiAliasing(bool);

			//camera
			void setCamera(Camera *);
			Camera *getCamera() const;

			//skybox
			void createSkybox(const std::vector<std::string> &);
			Skybox *getSkybox() const;

			//models
			Model *addModel(Model *);
			void removeModel(Model *);
			bool isModelExist(Model *);

			//geometry
			GeometryModel *addGeometry(GeometryModel *);
			void removeGeometry(GeometryModel *);
			void removeAllGeometries();

			//events
			bool onKeyDown(unsigned int);
			bool onKeyUp(unsigned int);
			bool onChar(unsigned int);
			bool onMouseMove(int, int);
			void onDisable();

			//scene
			void display(float);

		private:
			void createOrUpdateDeferredShadingShader();
			void onCameraProjectionUpdate();

			//scene
			void updateScene(float);
			void deferredGeometryRendering();
			void lightingPassRendering();
			void postUpdateScene();

			//scene properties
			unsigned int width, height;

			//managers
			ModelDisplayer *modelDisplayer;
			GeometryDisplayer *geometryDisplayer;
			OctreeManager<Model> *modelOctreeManager;

			LightManager *lightManager;
			bool isLightingActivated;

			ShadowManager *shadowManager;
			bool isShadowActivated;

			AmbientOcclusionManager *ambientOcclusionManager;
			bool isAmbientOcclusionActivated;

			AntiAliasingManager *antiAliasingManager;
			bool isAntiAliasingActivated;

			//camera
			Camera *camera;

			//skybox
			Skybox *skybox;

			//visual
			unsigned int *fboIDs;
			enum //FBO IDs indexes
			{
				FBO_SCENE = 0
			};

			unsigned int fboAttachments[3];
			unsigned int *textureIDs;
			enum //texture IDs indexes
			{
				TEX_DEPTH = 0,
				TEX_DIFFUSE,
				TEX_NORMAL_AND_AMBIENT,
				TEX_LIGHTING_PASS
			};

			std::shared_ptr<QuadDisplayer> lightingPassQuadDisplayer;
			unsigned int deferredShadingShader;
			int mInverseViewProjectionLoc, viewPositionLoc, hasLightingLoc, hasShadowLoc, hasAmbientOcclusionLoc;
	};

}

#endif
