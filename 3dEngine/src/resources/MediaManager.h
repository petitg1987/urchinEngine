#ifndef ENGINE_MEDIAMANAGER_H
#define ENGINE_MEDIAMANAGER_H

#include <map>
#include <string>
#include <stdexcept>
#include "UrchinCommon.h"

#include "resources/ResourceManager.h"
#include "resources/model/ConstMeshes.h"
#include "resources/model/ConstAnimation.h"
#include "resources/image/Image.h"
#include "resources/material/Material.h"
#include "resources/font/Font.h"
#include "resources/MediaManager.h"
#include "loader/Loader.h"

namespace urchin
{

	/**
	 * Find the appropriate loader according to the extension of the file and load the resource
	 */
	class MediaManager : public Singleton<MediaManager>
	{
		public:
			friend class Singleton<MediaManager>;
			
			template<class T> T* getMedia(const std::string &, void *params=nullptr);
			
		private:
			MediaManager();
			~MediaManager();

			std::map<std::string, LoaderInterface *> loadersRegistry;
	};

	#include "MediaManager.inl"

}

#endif
