#include "resources/MediaManager.h"
#include "loader/image/LoaderTGA.h"
#include "loader/model/LoaderMD5Mesh.h"
#include "loader/model/LoaderMD5Anim.h"
#include "loader/material/LoaderMTR.h"
#include "loader/font/LoaderTTF.h"

namespace urchin
{

	MediaManager::MediaManager()
	{
		loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("tga", new LoaderTGA));
		loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("md5mesh", new LoaderMD5Mesh));
		loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("md5anim", new LoaderMD5Anim));
		loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("mtr", new LoaderMTR));
		loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("ttf", new LoaderTTF));
	}
	
	MediaManager::~MediaManager()
	{
		for(std::map<std::string, LoaderInterface *> ::const_iterator it = loadersRegistry.begin(); it!=loadersRegistry.end(); ++it)
		{
			delete it->second;
		}
	}

}
