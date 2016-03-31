#include "system/FileSystem.h"
#include "system/FileHandler.h"

namespace urchin
{

	FileSystem::FileSystem() : Singleton<FileSystem>(),
			resourcesDirectory("./")
	{

	}

	FileSystem::~FileSystem()
	{

	}

	void FileSystem::setupResourcesDirectory(const std::string &resourcesDirectory)
	{
		this->resourcesDirectory = resourcesDirectory;
	}

	const std::string &FileSystem::getResourcesDirectory()
	{
		return resourcesDirectory;
	}

}
