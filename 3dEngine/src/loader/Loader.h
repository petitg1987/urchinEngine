#ifndef ENGINE_LOADER_H
#define ENGINE_LOADER_H

#include <string>
#include <stdexcept>

namespace urchin
{

	class LoaderInterface
	{

	};

	template<class T> class Loader : public LoaderInterface
	{
		public:
			virtual ~Loader();
		
			virtual T *loadFromFile(const std::string &filename, void *);
			virtual void saveToFile(const T *object, const std::string& filename);
	};

	#include "Loader.inl"
	
}

#endif
