#include "system/FileSystem.h"
#include "system/FileHandler.h"

namespace urchin
{

	FileSystem::FileSystem() : Singleton<FileSystem>(),
			workingDirectory("./")
	{

	}

	FileSystem::~FileSystem()
	{

	}

	/**
	 * Setup the wording directory from binary path. Working directory will the same as directory containing the binary.
	 */
	void FileSystem::setupWorkingDirectoryFromBinaryPath(const std::string &binaryPath)
	{
		this->workingDirectory = FileHandler::getDirectoryFrom(binaryPath);
	}

	void FileSystem::setupWorkingDirectory(const std::string &workingDirectory)
	{
		this->workingDirectory = workingDirectory;
	}

	/**
	 * @return Working directory (relative path to the binary directory)
	 */
	const std::string &FileSystem::getWorkingDirectory()
	{
		return workingDirectory;
	}

}
