#include "UrchinCommon.h"

#include "resources/ResourceManager.h"

namespace urchin
{
	
	ResourceManager::ResourceManager() : Singleton<ResourceManager>()
	{
		
	}

	ResourceManager::~ResourceManager()
	{
		if (!mResources.empty())
		{
			Logger::setLogger(new FileLogger());
			Logger::logger()<<Logger::prefix(Logger::LOG_WARNING);
			Logger::logger()<<"Resources not released:\n";
			for (std::map<std::string, Resource *>::const_iterator i = mResources.begin(); i != mResources.end(); ++i)
			{
				Logger::logger()<< " - " << i->second->getName() << "\n\n";
			}
			Logger::setLogger(nullptr);
		}
	}

	void ResourceManager::addResource(const std::string &name, Resource *resource)
	{
		mResources[name] = resource;
		resource->setName(name);
	}

	void ResourceManager::removeResource(const std::string &name)
	{
		std::map<std::string, Resource *>::iterator it = mResources.find(name);
		if(it!=mResources.end())
		{
			mResources.erase(it);
		}
	}

}
