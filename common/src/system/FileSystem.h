#ifndef ENGINE_FILESYSTEM_H
#define ENGINE_FILESYSTEM_H

#include <list>

#include "pattern/singleton/Singleton.h"

namespace urchin
{

	class FileSystem : public Singleton<FileSystem>
	{
		public:
			friend class Singleton<FileSystem>;

			void setupWorkingDirectoryFromBinaryPath(const std::string &);
			void setupWorkingDirectory(const std::string &);
			const std::string &getWorkingDirectory();

		private:
			FileSystem();
			virtual ~FileSystem();

			std::string workingDirectory;
	};

}

#endif
