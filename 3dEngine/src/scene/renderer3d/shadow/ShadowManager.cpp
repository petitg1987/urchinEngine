#include <GL/glew.h>
#include <cmath>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <limits>
#include <map>
#include <string>

#include "ShadowManager.h"
#include "scene/renderer3d/shadow/filter/ModelProduceShadowFilter.h"
#include "utils/filter/TextureFilter.h"
#include "utils/filter/gaussianblur/GaussianBlurFilterBuilder.h"
#include "utils/filter/downsample/DownSampleFilterBuilder.h"
#include "utils/display/geometry/obbox/OBBoxModel.h"

#define DEFAULT_NUMBER_SHADOW_MAPS 5
#define DEFAULT_SHADOW_MAP_RESOLUTION 1024
#define DEFAULT_VIEWING_SHADOW_DISTANCE 75.0
#define DEFAULT_BLUR_SHADOW BlurShadow::MEDIUM

namespace urchin
{

	ShadowManager::ShadowManager(LightManager *lightManager, OctreeManager<Model> *modelOctreeManager) :
			shadowMapBias(ConfigService::instance()->getFloatValue("shadow.shadowMapBias")),
			percentageUniformSplit(ConfigService::instance()->getFloatValue("shadow.frustumUniformSplitAgainstLogSplit")),
			lightViewOverflowStepSize(ConfigService::instance()->getFloatValue("shadow.lightViewOverflowStepSize")),
			shadowMapResolution(DEFAULT_SHADOW_MAP_RESOLUTION),
			nbShadowMaps(DEFAULT_NUMBER_SHADOW_MAPS),
			viewingShadowDistance(DEFAULT_VIEWING_SHADOW_DISTANCE),
			blurShadow(DEFAULT_BLUR_SHADOW),
			sceneWidth(0),
			sceneHeight(0),
			shadowModelDisplayer(nullptr),
			lightManager(lightManager),
			modelOctreeManager(modelOctreeManager),
			shadowUniform(nullptr),
			shadowModelUniform(nullptr),
			frustumDistance(0.0),
			bForceUpdateAllShadowMaps(false),
			depthSplitDistanceLoc(0),
			lightsLocation(nullptr)
	{
		switch(ConfigService::instance()->getUnsignedIntValue("shadow.depthComponent"))
		{
			case 16:
				depthComponent = GL_DEPTH_COMPONENT16;
				break;
			case 24:
				depthComponent = GL_DEPTH_COMPONENT24;
				break;
			case 32:
				depthComponent = GL_DEPTH_COMPONENT32;
				break;
			default:
				throw std::domain_error("Unsupported value for parameter 'shadow.depthComponent'.");
		}

		if(lightViewOverflowStepSize==0.0f)
		{ //because fmod function doesn't accept zero value.
			lightViewOverflowStepSize=std::numeric_limits<float>::epsilon();
		}

		lightManager->addObserver(this, LightManager::ADD_LIGHT);
		lightManager->addObserver(this, LightManager::REMOVE_LIGHT);

		createOrUpdateShadowModelDisplayer();
	}

	ShadowManager::~ShadowManager()
	{
		for (auto &shadowData : shadowDatas)
		{
			removeShadowMaps(shadowData.first);

			delete shadowData.second;
		}

		deleteLightsLocation();

		delete shadowModelDisplayer;
		delete shadowUniform;
		delete shadowModelUniform;
	}

	void ShadowManager::loadUniformLocationFor(unsigned int deferredShaderID)
	{
		//shadow information
		ShaderManager::instance()->bind(deferredShaderID);
		depthSplitDistanceLoc = static_cast<unsigned int>(glGetUniformLocation(deferredShaderID, "depthSplitDistance"));

		//light information
		deleteLightsLocation();
		lightsLocation = new LightLocation[lightManager->getMaxLights()];
		std::ostringstream shadowMapTextureLocName, mLightProjectionViewLocName;
		for(unsigned int i=0;i<lightManager->getMaxLights();++i)
		{
			//depth shadow texture
			shadowMapTextureLocName.str("");
			shadowMapTextureLocName << "lightsInfo[" << i << "].shadowMapTex";
			lightsLocation[i].shadowMapTexLoc = glGetUniformLocation(deferredShaderID, shadowMapTextureLocName.str().c_str());

			//light projection matrices
			lightsLocation[i].mLightProjectionViewLoc = new int[nbShadowMaps];
			for(unsigned int j=0; j<nbShadowMaps; ++j)
			{
				mLightProjectionViewLocName.str("");
				mLightProjectionViewLocName << "lightsInfo[" << i << "].mLightProjectionView[" << j << "]";
				lightsLocation[i].mLightProjectionViewLoc[j] = glGetUniformLocation(deferredShaderID, mLightProjectionViewLocName.str().c_str());
			}
		}
	}

	void ShadowManager::createOrUpdateShadowModelDisplayer()
	{
		std::map<std::string, std::string> geometryTokens, fragmentTokens;
		geometryTokens["MAX_VERTICES"] = std::to_string(3*nbShadowMaps);
		geometryTokens["NUMBER_SHADOW_MAPS"] = std::to_string(nbShadowMaps);
		delete shadowModelDisplayer;
		shadowModelDisplayer = new ModelDisplayer(ModelDisplayer::DEPTH_ONLY_MODE);
		shadowModelDisplayer->setCustomGeometryShader("modelShadowMap.geo", geometryTokens);
		shadowModelDisplayer->setCustomFragmentShader("modelShadowMap.frag", fragmentTokens);
		shadowModelDisplayer->initialize();

		delete shadowUniform;
		shadowUniform = new ShadowUniform();
		shadowUniform->setProjectionMatricesLocation(shadowModelDisplayer->getUniformLocation("projectionMatrix"));
		shadowModelDisplayer->setCustomUniform(shadowUniform);

		delete shadowModelUniform;
		shadowModelUniform = new ShadowModelUniform();
		shadowModelUniform->setLayersToUpdateLocation(shadowModelDisplayer->getUniformLocation("layersToUpdate"));
		shadowModelDisplayer->setCustomModelUniform(shadowModelUniform);
	}

	void ShadowManager::deleteLightsLocation()
	{
		if(lightsLocation!=nullptr)
		{
			for(unsigned int i=0;i<lightManager->getMaxLights();++i)
			{
				delete [] lightsLocation[i].mLightProjectionViewLoc;
			}
			delete [] lightsLocation;
		}
	}

	void ShadowManager::onResize(unsigned int width, unsigned int height)
	{
		sceneWidth = width;
		sceneHeight = height;

		for(std::map<const Light *, ShadowData *>::const_iterator it = shadowDatas.begin(); it!=shadowDatas.end(); ++it)
		{
			updateViewMatrix(it->first);
		}
	}

	void ShadowManager::onCameraProjectionUpdate(const Camera *camera)
	{
		this->projectionMatrix = camera->getProjectionMatrix();
		this->frustumDistance = camera->getFrustum().computeFarDistance() + camera->getFrustum().computeNearDistance();

		splitFrustum(camera->getFrustum());
		updateShadowLights();
	}

	void ShadowManager::notify(Observable *observable, int notificationType)
	{
		if(dynamic_cast<LightManager *>(observable)!=nullptr)
		{
			Light *light = lightManager->getLastUpdatedLight();
			switch(notificationType)
			{
				case LightManager::ADD_LIGHT:
				{
					light->addObserver(this, Light::PRODUCE_SHADOW);

					if(light->isProduceShadow())
					{
						addShadowLight(light);
					}
					break;
				}
				case LightManager::REMOVE_LIGHT:
				{
					light->removeObserver(this, Light::PRODUCE_SHADOW);

					if(light->isProduceShadow())
					{
						removeShadowLight(light);
					}
					break;
				}
				default:
					;
			}
		}else if(auto *light = dynamic_cast<Light *>(observable))
		{
			switch(notificationType)
			{
				case Light::LIGHT_MOVE:
				{
					updateViewMatrix(light);
					break;
				}
				case Light::PRODUCE_SHADOW:
				{
					if(light->isProduceShadow())
					{
						addShadowLight(light);
					}else
					{
						removeShadowLight(light);
					}
					break;
				}
				default:
					;
			}
		}
	}

	float ShadowManager::getShadowMapBias() const
	{
		return shadowMapBias;
	}

	void ShadowManager::setShadowMapResolution(unsigned int shadowMapResolution)
	{
		this->shadowMapResolution = shadowMapResolution;

		updateShadowLights();
	}

	unsigned int ShadowManager::getShadowMapResolution() const
	{
		return shadowMapResolution;
	}

	void ShadowManager::setNumberShadowMaps(unsigned int nbShadowMaps)
	{
		this->nbShadowMaps = nbShadowMaps;

		createOrUpdateShadowModelDisplayer();
		updateShadowLights();
		notifyObservers(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);
	}

	unsigned int ShadowManager::getNumberShadowMaps() const
	{
		return nbShadowMaps;
	}

	/**
	 * @param viewingShadowDistance Viewing shadow distance. If negative, shadow will be displayed until the far plane.
	 */
	void ShadowManager::setViewingShadowDistance(float viewingShadowDistance)
	{
		this->viewingShadowDistance = viewingShadowDistance;
	}

	/**
	 * @return Viewing shadow distance. If negative, shadow will be displayed until the far plane.
	 */
	float ShadowManager::getViewingShadowDistance() const
	{
		return viewingShadowDistance;
	}

	void ShadowManager::setBlurShadow(ShadowManager::BlurShadow blurShadow)
	{
		this->blurShadow = blurShadow;
		updateShadowLights();
	}

	ShadowManager::BlurShadow ShadowManager::getBlurShadow() const
	{
		return blurShadow;
	}

	const std::vector<Frustum<float>> &ShadowManager::getSplittedFrustums() const
	{
		return splittedFrustums;
	}

	const ShadowData &ShadowManager::getShadowData(const Light *light) const
	{
		auto it = shadowDatas.find(light);
		if(it==shadowDatas.end())
		{
			throw std::runtime_error("No shadow data found for this light.");
		}

		const ShadowData *shadowData = it->second;
		return *shadowData;
	}

	/**
	 * @return All visible models from all lights
	 */
	std::set<Model *> ShadowManager::getVisibleModels()
	{
		ScopeProfiler profiler("3d", "shadowGetVisibleModels");

		std::set<Model *> visibleModels;
		for(std::map<const Light *, ShadowData *>::const_iterator it = shadowDatas.begin(); it!=shadowDatas.end(); ++it)
		{
			for(unsigned int i=0; i<nbShadowMaps; ++i)
			{
				const std::set<Model *> &visibleModelsForLightInFrustumSplit = it->second->getFrustumShadowData(i)->getModels();
				visibleModels.insert(visibleModelsForLightInFrustumSplit.begin(), visibleModelsForLightInFrustumSplit.end());
			}
		}

		return visibleModels;
	}

	void ShadowManager::addShadowLight(const Light *light)
	{
		light->addObserver(this, Light::LIGHT_MOVE);

		shadowDatas[light] = new ShadowData(light, nbShadowMaps);

		createShadowMaps(light);
		updateViewMatrix(light);
	}

	void ShadowManager::removeShadowLight(const Light *light)
	{
		light->removeObserver(this, Light::LIGHT_MOVE);

		removeShadowMaps(light);

		delete shadowDatas[light];
		shadowDatas.erase(light);
	}

	/**
	 * Updates lights data which producing shadows
	 */
	void ShadowManager::updateShadowLights()
	{
		std::set<const Light *> allLights;
		for(std::map<const Light *, ShadowData *>::const_iterator it = shadowDatas.begin(); it!=shadowDatas.end(); ++it)
		{
			allLights.insert(it->first);
		}

		for (auto allLight : allLights)
		{
			removeShadowLight(allLight);
			addShadowLight(allLight);
		}
	}

	void ShadowManager::updateViewMatrix(const Light *light)
	{
		ShadowData *shadowData = shadowDatas[light];

		if(light->hasParallelBeams())
		{ //sun light
			Vector3<float> lightDirection = light->getDirections()[0];

			const Vector3<float> &f = lightDirection.normalize();
			const Vector3<float> &s = f.crossProduct(Vector3<float>(0.0, 1.0, 0.0)).normalize();
			const Vector3<float> &u = s.crossProduct(f).normalize();
			Matrix4<float> M(
				s[0],	s[1],	s[2],	0,
				u[0],	u[1],	u[2],	0,
				-f[0],	-f[1],	-f[2],	0,
				0,		0,		0,		1);

			Matrix4<float> eye;
			eye.buildTranslation(lightDirection.X, lightDirection.Y, lightDirection.Z);
			Matrix4<float> mViewShadow = M * eye;

			shadowData->setLightViewMatrix(mViewShadow);
		}else
		{
			throw std::runtime_error("Shadow currently not supported on omnidirectional light.");
		}
	}

	/**
	 * Updates frustum shadow data (models, shadow caster/receiver box, projection matrix)
	 */
	void ShadowManager::updateFrustumShadowData(const Light *light, ShadowData *shadowData)
	{
		if(light->hasParallelBeams())
		{ //sun light
            Matrix4<float> lightViewMatrixInverse = shadowData->getLightViewMatrix().inverse();
			for(unsigned int i=0; i<splittedFrustums.size(); ++i)
			{
				AABBox<float> aabboxSceneIndependent = createSceneIndependentBox(splittedFrustums[i], shadowData->getLightViewMatrix());
				OBBox<float> obboxSceneIndependentViewSpace = lightViewMatrixInverse * OBBox<float>(aabboxSceneIndependent);

				const std::set<Model *> models = modelOctreeManager->getOctreeablesIn(obboxSceneIndependentViewSpace, ModelProduceShadowFilter());
				shadowData->getFrustumShadowData(i)->updateModels(models);

				AABBox<float> aabboxSceneDependent = createSceneDependentBox(aabboxSceneIndependent, obboxSceneIndependentViewSpace,
						models, shadowData->getLightViewMatrix());
				shadowData->getFrustumShadowData(i)->updateShadowCasterReceiverBox(aabboxSceneDependent, bForceUpdateAllShadowMaps);
			}
		}else
		{
			throw std::runtime_error("Shadow not supported on omnidirectional light.");
		}

		bForceUpdateAllShadowMaps = false;
	}

	/**
	 * @return Box in light space containing shadow caster and receiver (scene independent)
	 */
	AABBox<float> ShadowManager::createSceneIndependentBox(const Frustum<float> &splittedFrustum, const Matrix4<float> &lightViewMatrix) const
	{
		const Frustum<float> &frustumLightSpace = lightViewMatrix * splittedFrustum;

		//determine point belonging to shadow caster/receiver box
		std::vector<Point3<float>> shadowReceiverAndCasterVertex;
		shadowReceiverAndCasterVertex.reserve(16);
		float nearCapZ = computeNearZForSceneIndependentBox(frustumLightSpace);
		for (const auto &frustumPoint : frustumLightSpace.getFrustumPoints())
		{
			//add shadow receiver points
			shadowReceiverAndCasterVertex.push_back(frustumPoint);

			//add shadow caster points
			shadowReceiverAndCasterVertex.emplace_back(Point3<float>(frustumPoint.X, frustumPoint.Y, nearCapZ));
		}

		//build shadow receiver/caster bounding box from points
		return AABBox<float>(shadowReceiverAndCasterVertex);
	}

	float ShadowManager::computeNearZForSceneIndependentBox(const Frustum<float> &splittedFrustumLightSpace) const
	{
		float nearestPointFromLight = splittedFrustumLightSpace.getFrustumPoints()[0].Z;
		for(unsigned int i=1; i<splittedFrustumLightSpace.getFrustumPoints().size(); ++i)
		{
			if(splittedFrustumLightSpace.getFrustumPoints()[i].Z > nearestPointFromLight)
			{
				nearestPointFromLight = splittedFrustumLightSpace.getFrustumPoints()[i].Z;
			}
		}
		return nearestPointFromLight + frustumDistance;
	}

	/**
	 * @return Box in light space containing shadow caster and receiver (scene dependent)
	 */
	AABBox<float> ShadowManager::createSceneDependentBox(const AABBox<float> &aabboxSceneIndependent, const OBBox<float> &obboxSceneIndependentViewSpace,
			const std::set<Model *> &models, const Matrix4<float> &lightViewMatrix) const
	{
		AABBox<float> aabboxSceneDependent;

        if(!models.empty())
        {
			bool boxInitialized = false;

            for (auto model : models)
            {
                const std::vector<AABBox<float>> &splittedAABBox = model->getSplittedAABBox();
                for (unsigned int i = 0; i < splittedAABBox.size(); ++i)
                {
                    if (splittedAABBox.size() == 1 || obboxSceneIndependentViewSpace.collideWithAABBox(splittedAABBox[i]))
                    {
                        if (boxInitialized)
                        {
                            aabboxSceneDependent = aabboxSceneDependent.merge(lightViewMatrix * splittedAABBox[i]);
                        } else
                        {
                            aabboxSceneDependent = lightViewMatrix * splittedAABBox[i];
                            boxInitialized = true;
                        }
                    }
                }
            }
        }

		Point3<float> cutMin(
		        std::min(std::max(aabboxSceneDependent.getMin().X, aabboxSceneIndependent.getMin().X), aabboxSceneIndependent.getMax().X),
                std::min(std::max(aabboxSceneDependent.getMin().Y, aabboxSceneIndependent.getMin().Y), aabboxSceneIndependent.getMax().Y),
				aabboxSceneIndependent.getMin().Z); //shadow can be projected outside the box: value cannot be capped

		Point3<float> cutMax(
				std::max(std::min(aabboxSceneDependent.getMax().X, aabboxSceneIndependent.getMax().X), aabboxSceneIndependent.getMin().X),
                std::max(std::min(aabboxSceneDependent.getMax().Y, aabboxSceneIndependent.getMax().Z), aabboxSceneIndependent.getMin().Y),
                std::max(std::min(aabboxSceneDependent.getMax().Z, aabboxSceneIndependent.getMax().Z), aabboxSceneIndependent.getMin().Z));

		cutMin.X = (cutMin.X<0.0f) ? cutMin.X-(lightViewOverflowStepSize+fmod(cutMin.X, lightViewOverflowStepSize)) : cutMin.X-fmod(cutMin.X, lightViewOverflowStepSize);
		cutMin.Y = (cutMin.Y<0.0f) ? cutMin.Y-(lightViewOverflowStepSize+fmod(cutMin.Y, lightViewOverflowStepSize)) : cutMin.Y-fmod(cutMin.Y, lightViewOverflowStepSize);
		cutMin.Z = (cutMin.Z<0.0f) ? cutMin.Z-(lightViewOverflowStepSize+fmod(cutMin.Z, lightViewOverflowStepSize)) : cutMin.Z-fmod(cutMin.Z, lightViewOverflowStepSize);
		cutMax.X = (cutMax.X<0.0f) ? cutMax.X-fmod(cutMax.X, lightViewOverflowStepSize) : cutMax.X+(lightViewOverflowStepSize-fmod(cutMax.X, lightViewOverflowStepSize));
		cutMax.Y = (cutMax.Y<0.0f) ? cutMax.Y-fmod(cutMax.Y, lightViewOverflowStepSize) : cutMax.Y+(lightViewOverflowStepSize-fmod(cutMax.Y, lightViewOverflowStepSize));
		cutMax.Z = (cutMax.Z<0.0f) ? cutMax.Z-fmod(cutMax.Z, lightViewOverflowStepSize) : cutMax.Z+(lightViewOverflowStepSize-fmod(cutMax.Z, lightViewOverflowStepSize));

		return AABBox<float>(cutMin, cutMax);
	}

	void ShadowManager::splitFrustum(const Frustum<float> &frustum)
	{
		ScopeProfiler profiler("3d", "splitFrustum");

		splittedDistance.clear();
		splittedFrustums.clear();

		float near = frustum.computeNearDistance();
		float far = viewingShadowDistance;
		if(viewingShadowDistance < 0.0f)
		{
			far = frustum.computeFarDistance();
		}

		float previousSplitDistance = near;

		for(unsigned int i=1; i<=nbShadowMaps; ++i)
		{
			float uniformSplit = near + (far - near) * (i/static_cast<float>(nbShadowMaps));
			float logarithmicSplit = near * std::pow(far/near, i/static_cast<float>(nbShadowMaps));

			float splitDistance = (percentageUniformSplit * uniformSplit) + ((1.0 - percentageUniformSplit) * logarithmicSplit);

			splittedDistance.push_back(splitDistance);
			splittedFrustums.push_back(frustum.splitFrustum(previousSplitDistance, splitDistance));

			previousSplitDistance = splitDistance;
		}
	}

	void ShadowManager::createShadowMaps(const Light *light)
	{
		//frame buffer object
		unsigned int fboID;
		glGenFramebuffers(1, &fboID);
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		shadowDatas[light]->setFboID(fboID);

		//textures for shadow map: depth texture && shadow map texture (variance shadow map)
		GLenum fboAttachments[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, fboAttachments);
		glReadBuffer(GL_NONE);

		unsigned int textureIDs[2];
		glGenTextures(2, &textureIDs[0]);

		glBindTexture(GL_TEXTURE_2D_ARRAY, textureIDs[0]);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, depthComponent, shadowMapResolution, shadowMapResolution, nbShadowMaps, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureIDs[0], 0);

		glBindTexture(GL_TEXTURE_2D_ARRAY, textureIDs[1]);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG32F, shadowMapResolution, shadowMapResolution, nbShadowMaps, 0, GL_RG, GL_FLOAT, nullptr);
		glFramebufferTexture(GL_FRAMEBUFFER, fboAttachments[0], textureIDs[1], 0);

		shadowDatas[light]->setDepthTextureID(textureIDs[0]);
		shadowDatas[light]->setShadowMapTextureID(textureIDs[1]);

		//add shadow map filter
		if(blurShadow!=BlurShadow::NO_BLUR)
		{
			std::shared_ptr<TextureFilter> verticalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>()
					->textureSize(shadowMapResolution, shadowMapResolution)
					->textureType(GL_TEXTURE_2D_ARRAY)
					->textureNumberLayer(nbShadowMaps)
					->textureInternalFormat(GL_RG32F)
					->textureFormat(GL_RG)
					->blurDirection(GaussianBlurFilterBuilder::VERTICAL_BLUR)
					->blurSize(static_cast<unsigned int>(blurShadow))
					->build();

			std::shared_ptr<TextureFilter> horizontalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>()
					->textureSize(shadowMapResolution, shadowMapResolution)
					->textureType(GL_TEXTURE_2D_ARRAY)
					->textureNumberLayer(nbShadowMaps)
					->textureInternalFormat(GL_RG32F)
					->textureFormat(GL_RG)
					->blurDirection(GaussianBlurFilterBuilder::HORIZONTAL_BLUR)
					->blurSize(static_cast<unsigned int>(blurShadow))
					->build();

			shadowDatas[light]->addTextureFilter(verticalBlurFilter);
			shadowDatas[light]->addTextureFilter(horizontalBlurFilter);
		}else
		{ //null filter necessary because it allow to store cached shadow map in a texture which is not cleared with a glClear().
			std::shared_ptr<TextureFilter> nullFilter = std::make_unique<DownSampleFilterBuilder>()
					->textureSize(shadowMapResolution, shadowMapResolution)
					->textureType(GL_TEXTURE_2D_ARRAY)
					->textureNumberLayer(nbShadowMaps)
					->textureInternalFormat(GL_RG32F)
					->textureFormat(GL_RG)
					->build();

			shadowDatas[light]->addTextureFilter(nullFilter);
		}

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ShadowManager::removeShadowMaps(const Light *light)
	{
		unsigned int depthTextureID = shadowDatas[light]->getDepthTextureID();
		glDeleteTextures(1, &depthTextureID);

		unsigned int shadowMapTextureID = shadowDatas[light]->getShadowMapTextureID();
		glDeleteTextures(1, &shadowMapTextureID);

		unsigned int frameBufferObjectID = shadowDatas[light]->getFboID();
		glDeleteFramebuffers(1, &frameBufferObjectID);
	}

	void ShadowManager::updateVisibleModels(const Frustum<float> &frustum)
	{
		ScopeProfiler profiler("3d", "shadowUpdateVisibleModels");

		splitFrustum(frustum);

		for(std::map<const Light *, ShadowData *>::const_iterator it = shadowDatas.begin(); it!=shadowDatas.end(); ++it)
		{
			updateFrustumShadowData(it->first, it->second);
		}
	}

	void ShadowManager::forceUpdateAllShadowMaps()
	{
		bForceUpdateAllShadowMaps = true;
	}

	void ShadowManager::updateShadowMaps()
	{
		ScopeProfiler profiler("3d", "updateShadowMaps");

		glBindTexture(GL_TEXTURE_2D, 0);

		for(std::map<const Light *, ShadowData *>::const_iterator it = shadowDatas.begin(); it!=shadowDatas.end(); ++it)
		{
			ShadowData *shadowData = it->second;

			glViewport(0, 0, shadowMapResolution, shadowMapResolution);
			glBindFramebuffer(GL_FRAMEBUFFER, shadowData->getFboID());
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			shadowUniform->setUniformData(shadowData);
			shadowModelUniform->setModelUniformData(shadowData);

			shadowModelDisplayer->setModels(shadowData->retrieveModels());
			shadowModelDisplayer->display(shadowData->getLightViewMatrix());

			shadowData->applyTextureFilters();
		}

		glViewport(0, 0, sceneWidth, sceneHeight);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	void ShadowManager::loadShadowMaps(const Matrix4<float> &viewMatrix, unsigned int shadowMapTextureUnitStart)
	{
		int i = 0;
		std::vector<Light *> visibleLights = lightManager->getVisibleLights();
		for(std::vector<Light *>::const_iterator itLights = visibleLights.begin(); itLights!=visibleLights.end(); ++itLights)
		{
			if((*itLights)->isProduceShadow())
			{
				std::map<const Light *, ShadowData *>::const_iterator it = shadowDatas.find(*itLights);
				const ShadowData *shadowData = it->second;

				unsigned int shadowMapTextureUnit = shadowMapTextureUnitStart + i;
				glActiveTexture(GL_TEXTURE0 + shadowMapTextureUnit);
				glBindTexture(GL_TEXTURE_2D_ARRAY, shadowData->getFilteredShadowMapTextureID());

				glUniform1i(lightsLocation[i].shadowMapTexLoc, shadowMapTextureUnit);

				for(unsigned int j=0; j<nbShadowMaps; ++j)
				{
					glUniformMatrix4fv(lightsLocation[i].mLightProjectionViewLoc[j], 1, static_cast<GLboolean>(false),
									   (float *)(shadowData->getFrustumShadowData(j)->getLightProjectionMatrix() * shadowData->getLightViewMatrix()));
				}
			}
			++i;
		}

		float depthSplitDistance[nbShadowMaps];
		for(unsigned int shadowMapIndex=0; shadowMapIndex<nbShadowMaps; ++shadowMapIndex)
		{
			float currSplitDistance = splittedDistance[shadowMapIndex];
			depthSplitDistance[shadowMapIndex] = ((projectionMatrix(2, 2)*-currSplitDistance + projectionMatrix(2, 3)) / (currSplitDistance)) / 2.0f + 0.5f;
		}

		glUniform1fv(depthSplitDistanceLoc, nbShadowMaps, depthSplitDistance);
	}

#ifdef _DEBUG
	void ShadowManager::drawLightSceneBox(const Frustum<float> &frustum, const Light *light, const Matrix4<float> &viewMatrix) const
	{
		auto itShadowData = shadowDatas.find(light);
		if(itShadowData==shadowDatas.end())
		{
			throw std::invalid_argument("shadow manager doesn't know this light.");
		}

		const Matrix4<float> &lightViewMatrix = itShadowData->second->getLightViewMatrix();
		AABBox<float> aabboxSceneIndependent = createSceneIndependentBox(frustum, lightViewMatrix);
		OBBox<float> obboxSceneIndependentViewSpace = lightViewMatrix.inverse() * OBBox<float>(aabboxSceneIndependent);

		const std::set<Model *> models = modelOctreeManager->getOctreeablesIn(obboxSceneIndependentViewSpace, ModelProduceShadowFilter());
		if(!models.empty())
		{
			AABBox<float> aabboxSceneDependent = createSceneDependentBox(aabboxSceneIndependent, obboxSceneIndependentViewSpace, models, lightViewMatrix);
			OBBox<float> obboxSceneDependentViewSpace = lightViewMatrix.inverse() * OBBox<float>(aabboxSceneDependent);

			OBBoxModel sceneDependentObboxModel(obboxSceneDependentViewSpace);
			sceneDependentObboxModel.onCameraProjectionUpdate(projectionMatrix);
			sceneDependentObboxModel.setColor(0.0, 1.0, 0.0);
			sceneDependentObboxModel.display(viewMatrix);
		}
	}
#endif

}
