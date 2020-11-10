#ifndef URCHINENGINE_RENDERER3D_H
#define URCHINENGINE_RENDERER3D_H

#include <string>
#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "scene/Renderer.h"
#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/model/displayer/ModelDisplayer.h"
#include "scene/renderer3d/postprocess/antialiasing/AntiAliasingManager.h"
#include "scene/renderer3d/postprocess/ambientocclusion/AmbientOcclusionManager.h"
#include "scene/renderer3d/lighting/shadow/ShadowManager.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/displayer/ModelDisplayer.h"
#include "scene/renderer3d/landscape/terrain/TerrainManager.h"
#include "scene/renderer3d/landscape/fog/FogManager.h"
#include "scene/renderer3d/landscape/water/WaterManager.h"
#include "scene/renderer3d/landscape/sky/SkyManager.h"
#include "resources/geometry/GeometryManager.h"
#include "resources/geometry/GeometryModel.h"
#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class Renderer3d : public Renderer, public Observer {
        public:
            Renderer3d();
            ~Renderer3d() override;

            //scene properties
            void onResize(unsigned int, unsigned int) override;
            void notify(Observable *, int) override;

            //managers
            OctreeManager<Model> *getModelOctreeManager() const;

            FogManager *getFogManager() const;

            TerrainManager *getTerrainManager() const;

            WaterManager *getWaterManager() const;

            SkyManager *getSkyManager() const;

            GeometryManager *getGeometryManager() const;

            LightManager *getLightManager() const;

            ShadowManager *getShadowManager() const;
            void activateShadow(bool);

            AmbientOcclusionManager *getAmbientOcclusionManager() const;
            void activateAmbientOcclusion(bool);

            AntiAliasingManager *getAntiAliasingManager() const;
            void activateAntiAliasing(bool);

            //camera
            void setCamera(Camera *);
            Camera *getCamera() const;

            //models
            void addModel(Model *);
            void removeModel(Model *);
            bool isModelExist(Model *);

            //events
            bool isPaused() const;
            bool onKeyPress(unsigned int) override;
            bool onKeyRelease(unsigned int) override;
            bool onChar(unsigned int) override;
            bool onMouseMove(int, int) override;
            void onDisable() override;

            //scene
            void pause();
            void unpause();
            void display(float) override;

        private:
            void createOrUpdateLightingShader();
            void onCameraProjectionUpdate();

            //model
            void updateModelsInFrustum();

            //scene
            void displayBuffers();
            void updateScene(float);
            void deferredGeometryRendering(float);
            void displayGeometryDetails();
            void lightingPassRendering();
            void postUpdateScene();

            //scene properties
            unsigned int sceneWidth, sceneHeight;
            bool paused;

            //managers
            ModelDisplayer *modelDisplayer;
            OctreeManager<Model> *modelOctreeManager;
            std::vector<Model *> modelsInFrustum;

            FogManager *fogManager;

            TerrainManager *terrainManager;

            WaterManager *waterManager;

            SkyManager *skyManager;

            GeometryManager *geometryManager;

            LightManager *lightManager;

            ShadowManager *shadowManager;
            bool isShadowActivated;

            AmbientOcclusionManager *ambientOcclusionManager;
            bool isAmbientOcclusionActivated;

            AntiAliasingManager *antiAliasingManager;
            bool isAntiAliasingActivated;

            //camera
            Camera *camera;

            //visual
            unsigned int *fboIDs;
            enum { //FBO IDs indices
                FBO_SCENE = 0
            };

            unsigned int fboAttachments[3];
            unsigned int *textureIDs;
            enum { //texture IDs indices
                TEX_DEPTH = 0,
                TEX_DIFFUSE,
                TEX_NORMAL_AND_AMBIENT,
                TEX_LIGHTING_PASS
            };

            std::unique_ptr<GenericRenderer> lightingRenderer;
            std::shared_ptr<Shader> lightingShader;
            ShaderVar mInverseViewProjectionShaderVar, viewPositionShaderVar;
    };

}

#endif
