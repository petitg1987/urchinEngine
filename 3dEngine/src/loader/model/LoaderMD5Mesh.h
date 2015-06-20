#ifndef ENGINE_LOADERMD5MESH_H
#define ENGINE_LOADERMD5MESH_H

#include <string>
#include <fstream>

#include "resources/model/ConstMeshes.h"
#include "loader/Loader.h"

namespace urchin
{
	
	class LoaderMD5Mesh : public Loader<ConstMeshes>
	{
		public:
			virtual ~LoaderMD5Mesh();

			ConstMeshes *loadFromFile(const std::string &, void *);
			
		private:
			void nextLine(std::ifstream &, std::string &);
	};

}

#endif
