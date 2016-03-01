#include "resources/MediaManager.h"
#include "loader/image/LoaderTGA.h"
#include "loader/model/LoaderUrchinMesh.h"
#include "loader/model/LoaderUrchinAnim.h"
#include "loader/material/LoaderMTR.h"
#include "loader/font/LoaderTTF.h"

namespace urchin
{

	MediaManager::MediaManager()
	{
		loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("tga", new LoaderTGA));
		loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("urchinMesh", new LoaderUrchinMesh));
		loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("urchinAnim", new LoaderUrchinAnim));
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
