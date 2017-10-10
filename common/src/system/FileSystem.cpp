#include "system/FileSystem.h"

namespace urchin
{

	FileSystem::FileSystem() : Singleton<FileSystem>(),
			resourcesDirectory("./")
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
