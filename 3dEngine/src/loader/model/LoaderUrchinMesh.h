#ifndef ENGINE_LOADERURCHINMESH_H
#define ENGINE_LOADERURCHINMESH_H

#include <string>
#include <fstream>

#include "resources/model/ConstMeshes.h"
#include "loader/Loader.h"

namespace urchin
{
	
	class LoaderUrchinMesh : public Loader<ConstMeshes>
	{
		public:
			virtual ~LoaderUrchinMesh();

			ConstMeshes *loadFromFile(const std::string &, void *);
			
		private:
			void nextLine(std::ifstream &, std::string &);
	};

}

#endif
