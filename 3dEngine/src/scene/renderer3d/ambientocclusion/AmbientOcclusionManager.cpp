#include <GL/gl.h>

#include "AmbientOcclusionManager.h"

namespace urchin
{

	AmbientOcclusionManager::AmbientOcclusionManager() :
		isInitialized(false),
		sceneWidth(0),
		sceneHeight(0)
	{

	}

	AmbientOcclusionManager::~AmbientOcclusionManager()
	{

	}

	void AmbientOcclusionManager::initialize()
	{
		if(isInitialized)
		{
			throw std::runtime_error("Ambient occlusion manager is already initialized.");
		}

		//TODO initialize

		isInitialized=true;
	}

	void AmbientOcclusionManager::onResize(int width, int height)
	{
		sceneWidth = width;
		sceneHeight = height;
	}

	void AmbientOcclusionManager::updateAOTexture()
	{
		//TODO update OA texture
	}

	void AmbientOcclusionManager::loadAOTexture()
	{
		//TODO load OA texture
	}

}
