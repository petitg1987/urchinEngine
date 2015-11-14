#ifndef ENGINE_RENDERER3D_H
#define ENGINE_RENDERER3D_H

#include <string>
#include <memory>
#include "UrchinCommon.h"

#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/camera/FpsCamera.h"
#include "scene/renderer3d/camera/FreeCamera.h"
#include "scene/renderer3d/model/displayer/ModelDisplayer.h"
#include "scene/renderer3d/antialiasing/AntiAliasingApplier.h"
#include "scene/renderer3d/octree/OctreeManager.h"
#include "scene/renderer3d/octree/Octreeable.h"
#include "scene/renderer3d/shadow/ShadowManager.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/skybox/Skybox.h"
#include "utils/display/geometry/GeometryDisplayer.h"
#include "utils/display/geometry/GeometryModel.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

	class Renderer3d
	{
		public:
			Renderer3d();
			virtual ~Renderer3d();

			//scene properties
			void initialize();
			void onResize(int, int);

			//managers
			LightManager *getLightManager() const;
			OctreeManager<Model> *getModelOctreeManager() const;
			ShadowManager *getShadowManager() const;

			//anti aliasing
			AntiAliasingApplier *getAntiAliasingApplier() const;
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

			//events
			void onKeyDown(unsigned int);
			void onKeyUp(unsigned int);
			void onMouseMove(int, int);
			void onKeyboardLocked(bool);

			//scene
			void display(float);

		private:
			void loadDeferredShadingShader();
			void onCameraProjectionUpdate();

			//scene
			void updateScene(float);
			void deferredGeometryRendering();
			void lightingPassRendering();

			//scene properties
			bool isInitialized;
			int width, height;

			//managers
			ModelDisplayer *modelDisplayer;
			GeometryDisplayer *geometryDisplayer;
			OctreeManager<Model> *modelOctreeManager;
			LightManager *lightManager;
			ShadowManager *shadowManager;

			//anti aliasing
			AntiAliasingApplier *antiAliasingApplier;
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

			GLenum fboAttachments[3];
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
			int depthTexLoc, diffuseTexLoc, normalAndAmbientTexLoc, mInverseViewProjectionLoc, viewPositionLoc;
	};

}

#endif
