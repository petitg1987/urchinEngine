#ifndef ENGINE_LOADERTTF_H
#define ENGINE_LOADERTTF_H

#include <string>

#include "resources/font/Font.h"
#include "loader/Loader.h"

namespace urchin
{

	class LoaderTTF : public Loader<Font>
	{
		public:
			virtual ~LoaderTTF();

			Font *loadFromFile(const std::string &, void *);
	};
	
}

#endif
