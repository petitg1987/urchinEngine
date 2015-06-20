#include "pattern/singleton/SingletonManager.h"

namespace urchin
{
	
	//static
	std::map<std::string, SingletonInterface *> SingletonManager::singletons;

	SingletonManager::SingletonManager()
	{

	}

	SingletonManager::~SingletonManager()
	{
		
	}

	void *SingletonManager::getSingleton(const std::string &name)
	{
		std::map<std::string, SingletonInterface *>::iterator it = singletons.find(name);
		if(it==singletons.end())
		{
			return nullptr;
		}
		return it->second;
	}

	void SingletonManager::addSingleton(const std::string &name, SingletonInterface *ptr)
	{
		singletons[name] = ptr;
	}

	void SingletonManager::destroyAllSingletons()
	{
		for(std::map<std::string, SingletonInterface *>::iterator it = singletons.begin(); it!=singletons.end(); ++it)
		{
			delete (*it).second;
		}
		singletons.clear();
	}

}
