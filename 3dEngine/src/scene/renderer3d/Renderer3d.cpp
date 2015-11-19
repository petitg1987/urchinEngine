#include <GL/gl.h>
#include <sstream>
#include <set>
#include <stdexcept>

#include "Renderer3d.h"
#include "utils/shader/ShaderManager.h"
#include "utils/shader/TokenReplacerShader.h"
#include "utils/display/texture/TextureDisplayer.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

#define DEFAULT_OCTREE_DEPTH 3

namespace urchin
{

	Renderer3d::Renderer3d() :
			isInitialized(false),
			width(0),
			height(0),
			modelDisplayer(nullptr),
			geometryDisplayer(nullptr),
			camera(nullptr),
			skybox(nullptr),
			fboIDs(nullptr),
			textureIDs(nullptr),
			deferredShadingShader(0),
			depthTexLoc(0),
			diffuseTexLoc(0),
			normalAndAmbientTexLoc(0),
			mInverseViewProjectionLoc(0),
			viewPositionLoc(0)
	{
		lightManager = new LightManager();
		modelOctreeManager = new OctreeManager<Model>(DEFAULT_OCTREE_DEPTH);
		shadowManager = new ShadowManager(lightManager, modelOctreeManager);

		antiAliasingApplier = new AntiAliasingApplier();
		isAntiAliasingActivated = true;
	}

	Renderer3d::~Renderer3d()
	{
		//models
		std::set<Model *> allOctreeableModels = modelOctreeManager->getOctreeables();
		for(std::set<Model *>::const_iterator it=allOctreeableModels.begin(); it!=allOctreeableModels.end(); ++it)
		{
			delete (*it);
		}

		//managers
		delete modelDisplayer;
		delete geometryDisplayer;
		delete shadowManager;
		delete modelOctreeManager;
		delete lightManager;

		//anti aliasing
		delete antiAliasingApplier;

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
			glDeleteTextures(3, textureIDs);
			delete [] textureIDs;
		}

		//deferred shading (pass 2)
		ShaderManager::instance()->removeProgram(deferredShadingShader);
	}

	void Renderer3d::initialize()
	{
		if(isInitialized)
		{
			throw std::runtime_error("Renderer is already initialized.");
		}

		//deferred shading (pass 1)
		fboIDs = new unsigned int[1];
		textureIDs = new unsigned int[4];
		modelDisplayer = new ModelDisplayer(ModelDisplayer::DEFAULT_MODE);
		modelDisplayer->initialize();
		geometryDisplayer = new GeometryDisplayer();
		glGenFramebuffers(1, fboIDs);
		glGenTextures(4, textureIDs);

		//deferred shading (pass 2)
		loadDeferredShadingShader();
		lightingPassQuadDisplayer = std::make_shared<QuadDisplayerBuilder>()
				->build();

		//managers
		lightManager->initialize(deferredShadingShader);
		modelOctreeManager->initialize();
		shadowManager->initialize(deferredShadingShader);

		//anti aliasing
		antiAliasingApplier->initialize();

		//default black skybox
		skybox = new Skybox();

		isInitialized = true;
	}

	void Renderer3d::loadDeferredShadingShader()
	{
		std::ostringstream maxLightsString, nbShadowMapsString;
		maxLightsString << lightManager->getMaxLights();
		nbShadowMapsString << shadowManager->getNumberShadowMaps();

		std::map<std::string, std::string> tokens;
		tokens["MAX_LIGHTS"] = maxLightsString.str();
		tokens["NUMBER_SHADOW_MAPS"] = nbShadowMapsString.str();
		tokens["OUTPUT_LOCATION"] = isAntiAliasingActivated ? "0" /*TEX_LIGHTING_PASS*/ : "0" /*Screen*/;

		ShaderManager::instance()->removeProgram(deferredShadingShader);
		deferredShadingShader = ShaderManager::instance()->createProgram("deferredShading.vert", "deferredShading.frag", tokens);
		ShaderManager::instance()->bind(deferredShadingShader);
		depthTexLoc = glGetUniformLocation(deferredShadingShader, "depthTex");
		diffuseTexLoc = glGetUniformLocation(deferredShadingShader, "colorTex");
		normalAndAmbientTexLoc = glGetUniformLocation(deferredShadingShader, "normalAndAmbientTex");
		mInverseViewProjectionLoc = glGetUniformLocation(deferredShadingShader, "mInverseViewProjection");
		viewPositionLoc = glGetUniformLocation(deferredShadingShader, "viewPosition");
	}

	void Renderer3d::onResize(int width, int height)
	{
		//scene properties
		if(!isInitialized)
		{
			return;
		}
		this->width = width;
		this->height = height;

		//camera
		if(camera)
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
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureIDs[TEX_DEPTH], 0);

		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DIFFUSE]); //diffuse buffer
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, fboAttachments[0], GL_TEXTURE_2D, textureIDs[TEX_DIFFUSE], 0);

		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_NORMAL_AND_AMBIENT]); //normal and ambient factor buffer
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, fboAttachments[1], GL_TEXTURE_2D, textureIDs[TEX_NORMAL_AND_AMBIENT], 0);

		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_LIGHTING_PASS]); //illuminated scene buffer
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, fboAttachments[2], GL_TEXTURE_2D, textureIDs[TEX_LIGHTING_PASS], 0);

		glReadBuffer(GL_NONE);

		//shadow
		shadowManager->onResize(width, height);

		//anti-aliasing
		antiAliasingApplier->onResize(width, height);
	}

	LightManager *Renderer3d::getLightManager() const
	{
		return lightManager;
	}

	OctreeManager<Model> *Renderer3d::getModelOctreeManager() const
	{
		return modelOctreeManager;
	}

	ShadowManager *Renderer3d::getShadowManager() const
	{
		return shadowManager;
	}

	AntiAliasingApplier *Renderer3d::getAntiAliasingApplier() const
	{
		return antiAliasingApplier;
	}

	void Renderer3d::activateAntiAliasing(bool isAntiAliasingActivated)
	{
		this->isAntiAliasingActivated = isAntiAliasingActivated;

		loadDeferredShadingShader();
	}

	void Renderer3d::setCamera(Camera *camera)
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Camera cannot be set because renderer is not initialized.");
		}

		this->camera = camera;
		if(camera)
		{
			onCameraProjectionUpdate();
		}
	}

	void Renderer3d::onCameraProjectionUpdate()
	{
		skybox->onCameraProjectionUpdate(camera);

		modelDisplayer->onCameraProjectionUpdate(camera);

		geometryDisplayer->onCameraProjectionUpdate(camera);

		shadowManager->onCameraProjectionUpdate(camera);
	}

	Camera *Renderer3d::getCamera() const
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Camera cannot be retrieved because renderer is not initialized.");
		}

		return camera;
	}

	void Renderer3d::createSkybox(const std::vector<std::string> &filenames)
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Skybox cannot be created because renderer is not initialized.");
		}

		delete skybox;
		skybox = new Skybox(filenames);
	}

	Skybox *Renderer3d::getSkybox() const
	{
		return skybox;
	}

	Model *Renderer3d::addModel(Model *model)
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Model cannot be added to the renderer because it's not initialized.");
		}

		if(model!=nullptr)
		{
			modelOctreeManager->addOctreeable(model);
		}
		return model;
	}

	void Renderer3d::removeModel(Model *model)
	{
		if(model!=nullptr)
		{
			modelOctreeManager->removeOctreeable(model);
			delete model;
		}
	}

	bool Renderer3d::isModelExist(Model *model)
	{
		std::set<Model *> allOctreeables = modelOctreeManager->getOctreeables();
		return allOctreeables.find(model)!=allOctreeables.end();
	}

	GeometryModel *Renderer3d::addGeometry(GeometryModel *geometry)
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Geometry cannot be added to the renderer because it's not initialized.");
		}

		geometryDisplayer->addGeometry(geometry);
		return geometry;
	}

	void Renderer3d::removeGeometry(GeometryModel *geometry)
	{
		if(geometry!=nullptr)
		{
			geometryDisplayer->removeGeometry(geometry);
		}
	}

	void Renderer3d::onKeyDown(unsigned int key)
	{
		if(camera!=nullptr && key<260)
		{
			camera->onKeyDown(key);
		}
	}

	void Renderer3d::onKeyUp(unsigned int key)
	{
		if(camera!=nullptr && key<260)
		{
			camera->onKeyUp(key);
		}
	}

	void Renderer3d::onMouseMove(int mouseX, int mouseY)
	{
		if(camera!=nullptr)
		{
			camera->onMouseMove(mouseX, mouseY);
		}
	}

	/**
	 * @param keyboardLocked Indicates if keyboard is locked by another resource
	 */
	void Renderer3d::onKeyboardLocked(bool keyboardLocked)
	{
		if(camera!=nullptr)
		{
			camera->onKeyboardLocked(keyboardLocked);
		}
	}

	void Renderer3d::display(float invFrameRate)
	{
		if(!camera)
		{ //nothing to display if camera doesn't exist
			return;
		}

		updateScene(invFrameRate);

		glBindFramebuffer(GL_FRAMEBUFFER, fboIDs[FBO_SCENE]);
		glDrawBuffers(2, &fboAttachments[0]);
		deferredGeometryRendering();

		if(isAntiAliasingActivated)
		{
			glDrawBuffers(1, &fboAttachments[2]);
			lightingPassRendering();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			antiAliasingApplier->applyOn(textureIDs[TEX_LIGHTING_PASS]);
		}else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			lightingPassRendering();
		}

		#ifdef _DEBUG
			//display depth buffer
//			TextureDisplayer textureDisplayer0(textureIDs[TEX_DEPTH], TextureDisplayer::DEPTH_FACTOR);
//			textureDisplayer0.setPosition(TextureDisplayer::LEFT, TextureDisplayer::TOP);
//			textureDisplayer0.initialize(width, height);
//			textureDisplayer0.display();

			//display color buffer
//			TextureDisplayer textureDisplayer1(textureIDs[TEX_DIFFUSE], TextureDisplayer::DEFAULT_FACTOR);
//			textureDisplayer1.setPosition(TextureDisplayer::CENTER_X, TextureDisplayer::TOP);
//			textureDisplayer1.initialize(width, height);
//			textureDisplayer1.display();

			//display normal and ambient buffer
//			TextureDisplayer textureDisplayer2(textureIDs[TEX_NORMAL_AND_AMBIENT], TextureDisplayer::DEFAULT_FACTOR);
//			textureDisplayer2.setPosition(TextureDisplayer::RIGHT, TextureDisplayer::TOP);
//			textureDisplayer2.initialize(width, height);
//			textureDisplayer2.display();

			//display illuminated scene buffer
//			TextureDisplayer textureDisplayer3(textureIDs[TEX_LIGHTING_PASS], TextureDisplayer::DEFAULT_FACTOR);
//			textureDisplayer3.setPosition(TextureDisplayer::LEFT, TextureDisplayer::BOTTOM);
//			textureDisplayer3.initialize(width, height);
//			textureDisplayer3.display();

			//display shadow map
//			const Light *firstLight = lightManager->getVisibleLights()[0]; //choose light
//			const unsigned int shadowMapNumber = 0; //choose shadow map to display [0, nbShadowMaps-1]
//			unsigned int shadowMapTextureID = shadowManager->getShadowData(firstLight).getShadowMapTextureID();
//			TextureDisplayer textureDisplayer4(shadowMapTextureID, shadowMapNumber, TextureDisplayer::DEFAULT_FACTOR);
//			textureDisplayer4.setPosition(TextureDisplayer::CENTER_X, TextureDisplayer::BOTTOM);
//			textureDisplayer4.initialize(width, height);
//			textureDisplayer4.display();
		#endif
	}

	void Renderer3d::updateScene(float invFrameRate)
	{
		//move the camera
		camera->updateCameraView(invFrameRate);

		//determine visible lights on scene
		lightManager->updateLights(camera->getFrustum());

		//animate models (only one visible to scene OR producing shadow on scene)
		shadowManager->updateVisibleModels(camera->getFrustum());
		modelDisplayer->setModels(shadowManager->getVisibleModels());
		modelDisplayer->updateAnimation(invFrameRate);

		//update shadow maps
		shadowManager->updateShadowMaps();
	}

	/**
	 * First pass of deferred shading algorithm.
	 * Render depth, color, normal, etc. into buffers.
	 */
	void Renderer3d::deferredGeometryRendering()
	{
		glClear(GL_DEPTH_BUFFER_BIT);

		skybox->display(camera->getViewMatrix(), camera->getPosition());

		modelDisplayer->setModels(modelOctreeManager->getOctreeablesIn(getCamera()->getFrustum()));
		modelDisplayer->display(camera->getViewMatrix());

		geometryDisplayer->display(camera->getViewMatrix());

		#ifdef _DEBUG
			//display the octree
//			modelOctreeManager->drawOctree(camera->getProjectionMatrix(), camera->getViewMatrix());

			//display models bounding box
//			modelDisplayer->drawBBox(camera->getProjectionMatrix(), camera->getViewMatrix());

			//display light octree
//			lightManager->drawLightOctree(camera->getProjectionMatrix(), camera->getViewMatrix());

			//display scene box visible from light based on input (splitted) frustum
//			const Light *firstLight = lightManager->getVisibleLights()[0]; //choose light
//			Frustum<float> frustum(
//				Point3<float>(-14.1412, -1.43174, -8.4988),
//				Point3<float>(-14.4282, -1.43174, -8.43292),
//				Point3<float>(-14.1672, -1.54952, -8.61236),
//				Point3<float>(-14.4543, -1.54952, -8.54648),
//				Point3<float>(-8.16763, -4.02435, -3.34841),
//				Point3<float>(-17.5582, -4.02439, -1.19325),
//				Point3<float>(-9.02018, -7.87719, -7.06327),
//				Point3<float>(-18.4108, -7.87724, -4.90811));
//			shadowManager->drawLightSceneBox(frustum, firstLight, camera->getViewMatrix());
		#endif
	}

	/**
	 * Second pass of deferred shading algorithm.
	 * Compute lighting in pixel shader and render the scene to screen.
	 */
	void Renderer3d::lightingPassRendering()
	{
		ShaderManager::instance()->bind(deferredShadingShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DEPTH]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DIFFUSE]);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_NORMAL_AND_AMBIENT]);

		glUniform1i(depthTexLoc, 0);
		glUniform1i(diffuseTexLoc, 1);
		glUniform1i(normalAndAmbientTexLoc, 2);

		glUniformMatrix4fv(mInverseViewProjectionLoc, 1, false, (const float*) (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse());
		glUniform3fv(viewPositionLoc, 1, (const float *)camera->getPosition());

		lightManager->loadLights();
		shadowManager->loadShadowMaps(camera->getViewMatrix());

		lightingPassQuadDisplayer->display();
	}

}
