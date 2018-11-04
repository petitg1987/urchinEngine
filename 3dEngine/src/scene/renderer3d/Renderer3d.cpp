#include <GL/glew.h>
#include <set>
#include <stdexcept>
#include <locale>

#include "Renderer3d.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"
#include "utils/display/texture/TextureDisplayer.h"

#define DEFAULT_OCTREE_MIN_SIZE 20.0f

namespace urchin
{

	Renderer3d::Renderer3d() :
			width(0),
			height(0),
			modelDisplayer(nullptr),
			fogManager(nullptr),
			terrainManager(nullptr),
			waterManager(nullptr),
			geometryManager(nullptr),
			camera(nullptr),
			skybox(nullptr),
			fboIDs(nullptr),
			textureIDs(nullptr),
			deferredShadingShader(0),
			mInverseViewProjectionLoc(0),
			viewPositionLoc(0)
	{
		//deferred shading (pass 1)
		fboIDs = new unsigned int[1];
		textureIDs = new unsigned int[4];
		modelDisplayer = new ModelDisplayer(ModelDisplayer::DEFAULT_MODE);
		modelDisplayer->initialize();
		glGenFramebuffers(1, fboIDs);
		glGenTextures(4, textureIDs);

		modelOctreeManager = new OctreeManager<Model>(DEFAULT_OCTREE_MIN_SIZE);

		fogManager = new FogManager();

		terrainManager = new TerrainManager();

		waterManager = new WaterManager();

		geometryManager = new GeometryManager();

		lightManager = new LightManager();

		shadowManager = new ShadowManager(lightManager, modelOctreeManager);
		shadowManager->addObserver(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);
		isShadowActivated = true;

		ambientOcclusionManager = new AmbientOcclusionManager(textureIDs[TEX_DEPTH], textureIDs[TEX_NORMAL_AND_AMBIENT]);
		isAmbientOcclusionActivated = true;

		//deferred shading (pass 2)
		createOrUpdateDeferredShadingShader();
		lightingPassQuadDisplayer = std::make_unique<QuadDisplayerBuilder>()->build();

		antiAliasingManager = new AntiAliasingManager();
		isAntiAliasingActivated = true;

		//default black skybox
		skybox = new Skybox();
	}

	Renderer3d::~Renderer3d()
	{
		//models
		std::set<Model *> allOctreeableModels = modelOctreeManager->getOctreeables();
		for (auto allOctreeableModel : allOctreeableModels)
		{
			delete allOctreeableModel;
		}

		//managers
		delete modelDisplayer;
		delete waterManager;
		delete terrainManager;
		delete fogManager;
		delete geometryManager;
		delete shadowManager;
		delete modelOctreeManager;
		delete lightManager;
		delete ambientOcclusionManager;
		delete antiAliasingManager;

		//skybox
		delete skybox;

		//deferred shading (pass 1)
		if(fboIDs!=nullptr)
		{
			glDeleteFramebuffers(1, fboIDs);
			delete [] fboIDs;
		}
		if(textureIDs!=nullptr)
		{
			glDeleteTextures(4, textureIDs);
			delete [] textureIDs;
		}

		//deferred shading (pass 2)
		ShaderManager::instance()->removeProgram(deferredShadingShader);
	}

	void Renderer3d::createOrUpdateDeferredShadingShader()
	{
		std::locale::global(std::locale("C")); //for float

		std::map<std::string, std::string> tokens;
		tokens["MAX_LIGHTS"] = std::to_string(lightManager->getMaxLights());
		tokens["NUMBER_SHADOW_MAPS"] = std::to_string(shadowManager->getNumberShadowMaps());
		tokens["SHADOW_MAP_BIAS"] = std::to_string(shadowManager->getShadowMapBias());
		tokens["OUTPUT_LOCATION"] = "0"; // isAntiAliasingActivated ? "0" /*TEX_LIGHTING_PASS*/ : "0" /*Screen*/;
		ShaderManager::instance()->removeProgram(deferredShadingShader);
		deferredShadingShader = ShaderManager::instance()->createProgram("deferredShading.vert", "", "deferredShading.frag", tokens);
		ShaderManager::instance()->bind(deferredShadingShader);

		int depthTexLoc = glGetUniformLocation(deferredShadingShader, "depthTex");
		glUniform1i(depthTexLoc, GL_TEXTURE0-GL_TEXTURE0);
		int diffuseTexLoc = glGetUniformLocation(deferredShadingShader, "colorTex");
		glUniform1i(diffuseTexLoc, GL_TEXTURE1-GL_TEXTURE0);
		int normalAndAmbientTexLoc = glGetUniformLocation(deferredShadingShader, "normalAndAmbientTex");
		glUniform1i(normalAndAmbientTexLoc, GL_TEXTURE2-GL_TEXTURE0);
		int hasShadowLoc = glGetUniformLocation(deferredShadingShader, "hasShadow");
		glUniform1i(hasShadowLoc, isShadowActivated);
		int hasAmbientOcclusionLoc = glGetUniformLocation(deferredShadingShader, "hasAmbientOcclusion");
		glUniform1i(hasAmbientOcclusionLoc, isAmbientOcclusionActivated);
		mInverseViewProjectionLoc = glGetUniformLocation(deferredShadingShader, "mInverseViewProjection");
		viewPositionLoc = glGetUniformLocation(deferredShadingShader, "viewPosition");

		//managers
		fogManager->loadUniformLocationFor(deferredShadingShader);
		lightManager->loadUniformLocationFor(deferredShadingShader);
		shadowManager->loadUniformLocationFor(deferredShadingShader);
		ambientOcclusionManager->loadUniformLocationFor(deferredShadingShader);
	}

	void Renderer3d::onResize(unsigned int width, unsigned int height)
	{
		//scene properties
		this->width = width;
		this->height = height;

		//camera
		if(camera!=nullptr)
		{
			camera->onResize(width, height);
			onCameraProjectionUpdate();
		}

		//deferred shading
		glBindFramebuffer(GL_FRAMEBUFFER, fboIDs[FBO_SCENE]);
		fboAttachments[0] = GL_COLOR_ATTACHMENT0;
		fboAttachments[1] = GL_COLOR_ATTACHMENT1;
		fboAttachments[2] = GL_COLOR_ATTACHMENT2;

		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DEPTH]); //depth buffer
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureIDs[TEX_DEPTH], 0);

		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DIFFUSE]); //diffuse buffer
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, fboAttachments[0], GL_TEXTURE_2D, textureIDs[TEX_DIFFUSE], 0);

		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_NORMAL_AND_AMBIENT]); //normal and ambient factor buffer
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, fboAttachments[1], GL_TEXTURE_2D, textureIDs[TEX_NORMAL_AND_AMBIENT], 0);

		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_LIGHTING_PASS]); //illuminated scene buffer
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, fboAttachments[2], GL_TEXTURE_2D, textureIDs[TEX_LIGHTING_PASS], 0);

		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//manager
		shadowManager->onResize(width, height);
		ambientOcclusionManager->onResize(width, height);
		antiAliasingManager->onResize(width, height);
	}

	void Renderer3d::notify(Observable *observable, int notificationType)
	{
		if(dynamic_cast<ShadowManager *>(observable)!=nullptr)
		{
			switch(notificationType)
			{
				case ShadowManager::NUMBER_SHADOW_MAPS_UPDATE:
				{
					createOrUpdateDeferredShadingShader();
					break;
				}
				default:
					;
			}
		}
	}

	OctreeManager<Model> *Renderer3d::getModelOctreeManager() const
	{
		return modelOctreeManager;
	}

	FogManager *Renderer3d::getFogManager() const
	{
		return fogManager;
	}

	TerrainManager *Renderer3d::getTerrainManager() const
	{
		return terrainManager;
	}

	WaterManager *Renderer3d::getWaterManager() const
	{
		return waterManager;
	}

	GeometryManager *Renderer3d::getGeometryManager() const
	{
		return geometryManager;
	}

	LightManager *Renderer3d::getLightManager() const
	{
		return lightManager;
	}

	ShadowManager *Renderer3d::getShadowManager() const
	{
		return shadowManager;
	}

	void Renderer3d::activateShadow(bool isShadowActivated)
	{
		this->isShadowActivated = isShadowActivated;

		createOrUpdateDeferredShadingShader();
		shadowManager->forceUpdateAllShadowMaps();
	}

	AmbientOcclusionManager *Renderer3d::getAmbientOcclusionManager() const
	{
		return ambientOcclusionManager;
	}

	void Renderer3d::activateAmbientOcclusion(bool isAmbientOcclusionActivated)
	{
		this->isAmbientOcclusionActivated = isAmbientOcclusionActivated;

		createOrUpdateDeferredShadingShader();
	}

	AntiAliasingManager *Renderer3d::getAntiAliasingManager() const
	{
		return antiAliasingManager;
	}

	void Renderer3d::activateAntiAliasing(bool isAntiAliasingActivated)
	{
		this->isAntiAliasingActivated = isAntiAliasingActivated;
	}

	void Renderer3d::setCamera(Camera *camera)
	{
		this->camera = camera;
		if(camera!=nullptr)
		{
			onCameraProjectionUpdate();
		}
	}

	void Renderer3d::onCameraProjectionUpdate()
	{
		skybox->onCameraProjectionUpdate(camera);

		modelDisplayer->onCameraProjectionUpdate(camera);

		terrainManager->onCameraProjectionUpdate(camera);

		waterManager->onCameraProjectionUpdate(camera);

		geometryManager->onCameraProjectionUpdate(camera);

		shadowManager->onCameraProjectionUpdate(camera);

		ambientOcclusionManager->onCameraProjectionUpdate(camera);
	}

	Camera *Renderer3d::getCamera() const
	{
		return camera;
	}

	void Renderer3d::createSkybox(const std::vector<std::string> &filenames)
	{
		delete skybox;
		skybox = new Skybox(filenames);
	}

	Skybox *Renderer3d::getSkybox() const
	{
		return skybox;
	}

	void Renderer3d::addModel(Model *model)
	{
		if(model!=nullptr)
		{
			modelOctreeManager->addOctreeable(model);
		}
	}

	void Renderer3d::removeModel(Model *model)
	{
		if(model!=nullptr)
		{
			modelOctreeManager->removeOctreeable(model);
		}
		delete model;
	}

	bool Renderer3d::isModelExist(Model *model)
	{
		std::set<Model *> allOctreeables = modelOctreeManager->getOctreeables();
		return allOctreeables.find(model)!=allOctreeables.end();
	}

	bool Renderer3d::onKeyDown(unsigned int key)
	{
		if(camera!=nullptr && key<260)
		{
			camera->onKeyDown(key);
		}
		return true;
	}

	bool Renderer3d::onKeyUp(unsigned int key)
	{
		if(camera!=nullptr && key<260)
		{
			camera->onKeyUp(key);
		}
		return true;
	}

	bool Renderer3d::onChar(unsigned int)
	{
		//nothing to do
		return true;
	}

	bool Renderer3d::onMouseMove(int mouseX, int mouseY)
	{
		if(camera!=nullptr)
		{
			camera->onMouseMove(mouseX, mouseY);
		}
		return true;
	}

	void Renderer3d::onDisable()
	{
		//nothing to do
	}

	void Renderer3d::display(float invFrameRate)
	{
		ScopeProfiler profiler("3d", "renderer3dDisplay");

		if(camera==nullptr)
		{ //nothing to display if camera doesn't exist
			return;
		}

		updateScene(invFrameRate);

		glBindFramebuffer(GL_FRAMEBUFFER, fboIDs[FBO_SCENE]);
		glDrawBuffers(2, &fboAttachments[0]);
		deferredGeometryRendering(invFrameRate);

		if(isAntiAliasingActivated)
		{
			glDrawBuffers(1, &fboAttachments[2]);
			lightingPassRendering();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			antiAliasingManager->applyOn(textureIDs[TEX_LIGHTING_PASS]);
		}else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			lightingPassRendering();
		}

		postUpdateScene();

		#ifdef _DEBUG
			//display depth buffer
//			float depthIntensity = 5.0f;
//			TextureDisplayer textureDisplayer0(textureIDs[TEX_DEPTH], TextureDisplayer::DEPTH_VALUE, depthIntensity);
//			textureDisplayer0.setPosition(TextureDisplayer::LEFT, TextureDisplayer::TOP);
//			textureDisplayer0.initialize(width, height, camera->getNearPlane(), camera->getFarPlane());
//			textureDisplayer0.display();

			//display color buffer
//			TextureDisplayer textureDisplayer1(textureIDs[TEX_DIFFUSE], TextureDisplayer::DEFAULT_VALUE);
//			textureDisplayer1.setPosition(TextureDisplayer::CENTER_X, TextureDisplayer::TOP);
//			textureDisplayer1.initialize(width, height, camera->getNearPlane(), camera->getFarPlane());
//			textureDisplayer1.display();

//			//display normal and ambient buffer
//			TextureDisplayer textureDisplayer2(textureIDs[TEX_NORMAL_AND_AMBIENT], TextureDisplayer::DEFAULT_VALUE);
//			textureDisplayer2.setPosition(TextureDisplayer::RIGHT, TextureDisplayer::TOP);
//			textureDisplayer2.initialize(width, height, camera->getNearPlane(), camera->getFarPlane());
//			textureDisplayer2.display();

			//display illuminated scene buffer
//			TextureDisplayer textureDisplayer3(textureIDs[TEX_LIGHTING_PASS], TextureDisplayer::DEFAULT_VALUE);
//			textureDisplayer3.setPosition(TextureDisplayer::LEFT, TextureDisplayer::BOTTOM);
//			textureDisplayer3.initialize(width, height, camera->getNearPlane(), camera->getFarPlane());
//			textureDisplayer3.display();

			//display shadow map
//			const Light *firstLight = lightManager->getVisibleLights()[0]; //choose light
//			const unsigned int shadowMapNumber = 0; //choose shadow map to display [0, nbShadowMaps-1]
//			unsigned int shadowMapTextureID = shadowManager->getShadowData(firstLight).getShadowMapTextureID();
//			TextureDisplayer textureDisplayer4(shadowMapTextureID, shadowMapNumber, TextureDisplayer::DEFAULT_VALUE);
//			textureDisplayer4.setPosition(TextureDisplayer::CENTER_X, TextureDisplayer::BOTTOM);
//			textureDisplayer4.initialize(width, height, camera->getNearPlane(), camera->getFarPlane());
//			textureDisplayer4.display();

//			//display ambient occlusion buffer
//			float ambientOcclusionIntensity = 10.0f;
//			TextureDisplayer textureDisplayer5(ambientOcclusionManager->getAmbientOcclusionTextureID(), TextureDisplayer::INVERSE_GRAYSCALE_VALUE, ambientOcclusionIntensity);
//			textureDisplayer5.setPosition(TextureDisplayer::RIGHT, TextureDisplayer::BOTTOM);
//			textureDisplayer5.initialize(width, height, camera->getNearPlane(), camera->getFarPlane());
//			textureDisplayer5.display();
		#endif
	}

	void Renderer3d::updateScene(float invFrameRate)
	{
		ScopeProfiler profiler("3d", "updateScene");

		//move the camera
		camera->updateCameraView(invFrameRate);

		//refresh models in octree
		modelOctreeManager->refreshOctreeables();

		//determine visible lights on scene
		lightManager->updateLights(camera->getFrustum());

		//determine models producing shadow on scene
		if(isShadowActivated)
		{
			shadowManager->updateVisibleModels(camera->getFrustum());
		}

		//animate models (only those visible to scene OR producing shadow on scene)
		if(isShadowActivated)
		{
			modelDisplayer->setModels(shadowManager->getVisibleModels());
		}else
		{
			modelDisplayer->setModels(modelOctreeManager->getOctreeablesIn(getCamera()->getFrustum()));
		}
		modelDisplayer->updateAnimation(invFrameRate);

		//update shadow maps
		if(isShadowActivated)
		{
			shadowManager->updateShadowMaps();
		}
	}

	/**
	 * First pass of deferred shading algorithm.
	 * Render depth, color, normal, etc. into buffers.
	 */
	void Renderer3d::deferredGeometryRendering(float invFrameRate)
	{
		ScopeProfiler profiler("3d", "deferredGeometryRendering");

		glClear(GL_DEPTH_BUFFER_BIT);

		skybox->display(camera->getViewMatrix(), camera->getPosition());

		modelDisplayer->setModels(modelOctreeManager->getOctreeablesIn(getCamera()->getFrustum()));
		modelDisplayer->display(camera->getViewMatrix());

		terrainManager->display(camera, invFrameRate);

		waterManager->display(camera, fogManager, invFrameRate);

		geometryManager->display(camera->getViewMatrix());

		if(isAmbientOcclusionActivated)
		{
			ambientOcclusionManager->updateAOTexture(camera);
		}

		#ifdef _DEBUG
			//display the octree
//			modelOctreeManager->drawOctree(camera->getProjectionMatrix(), camera->getViewMatrix());

			//display models bounding box
//			modelDisplayer->drawBBox(camera->getProjectionMatrix(), camera->getViewMatrix());

			//display bones of specified model in base position
//			modelDisplayer->drawBaseBones(camera->getProjectionMatrix(), camera->getViewMatrix(), "models/character.urchinMesh");

			//display light octree
//			lightManager->drawLightOctree(camera->getProjectionMatrix(), camera->getViewMatrix());

			//display scene box visible from light based on splitted frustums
//			const Light *firstLight = lightManager->getVisibleLights()[0]; //choose light
//			for(auto &frustum : shadowManager->getSplittedFrustums())
//			{
//				shadowManager->drawLightSceneBox(frustum, firstLight, camera->getViewMatrix());
//			}

		#endif
	}

	/**
	 * Second pass of deferred shading algorithm.
	 * Compute lighting in pixel shader and render the scene to screen.
	 */
	void Renderer3d::lightingPassRendering()
	{
        ScopeProfiler profiler("3d", "lightingPassRendering");

		ShaderManager::instance()->bind(deferredShadingShader);
		unsigned int nextTextureUnit = 0;

		glActiveTexture(GL_TEXTURE0 + nextTextureUnit++);
		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DEPTH]);

		glActiveTexture(GL_TEXTURE0 + nextTextureUnit++);
		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DIFFUSE]);

		glActiveTexture(GL_TEXTURE0 + nextTextureUnit++);
		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_NORMAL_AND_AMBIENT]);

		glUniformMatrix4fv(mInverseViewProjectionLoc, 1, GL_FALSE, (const float*) (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse());
		glUniform3fv(viewPositionLoc, 1, (const float *)camera->getPosition());

        lightManager->loadLights();

		if(isShadowActivated)
		{
			shadowManager->loadShadowMaps(camera->getViewMatrix(), nextTextureUnit);
			nextTextureUnit += shadowManager->getNumberShadowMaps();
		}

		if(isAmbientOcclusionActivated)
		{
			ambientOcclusionManager->loadAOTexture(nextTextureUnit);
            //nextTextureUnit++;
		}

		lightingPassQuadDisplayer->display();
	}

	void Renderer3d::postUpdateScene()
	{
        ScopeProfiler profiler("3d", "postUpdateScene");

		modelOctreeManager->postRefreshOctreeables();

		lightManager->postUpdateLights();
	}

}
