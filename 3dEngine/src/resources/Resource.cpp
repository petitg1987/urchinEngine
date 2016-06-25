#include "resources/Resource.h"
#include "resources/ResourceManager.h"

namespace urchin
{
	
	Resource::Resource() : name(""), refCount(1)
	{
		
	}

	Resource::~Resource()
	{
		if(!name.empty())
		{
			ResourceManager::instance()->removeResource(name);
		}
	}

	const std::string& Resource::getName() const
	{
		return name;
	}

	void Resource::setName(const std::string &nameU)
	{
		name = nameU;
	}

	unsigned int Resource::getRefCount() const
	{
		return refCount;
	}

	void Resource::addRef()
	{
		++refCount;
	}

	void Resource::release()
	{	
		if(--refCount==0)
		{
			delete this;
		}
	}

	/**
	* Deleter for std::shared_ptr
	*/
	void Resource::ResourceDeleter::operator()(Resource *const resource)
	{
		resource->release();
	}

}
