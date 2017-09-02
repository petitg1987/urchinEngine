#ifndef URCHINENGINE_LOADERMTR_H
#define URCHINENGINE_LOADERMTR_H

#include <string>

#include "resources/material/Material.h"
#include "loader/Loader.h"

namespace urchin
{
	
	class LoaderMTR : public Loader<Material>
	{
		public:
			virtual ~LoaderMTR() = default;

			Material *loadFromFile(const std::string &, void *);
	};

}

#endif
