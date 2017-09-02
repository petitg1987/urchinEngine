#ifndef URCHINENGINE_TEXTUREMANAGER_H
#define URCHINENGINE_TEXTUREMANAGER_H

#include <list>
#include "UrchinCommon.h"

namespace urchin
{

	/**
	* A texture manager global to all renderer
	*/
	class TextureManager : public Singleton<TextureManager>
	{
		public:
			friend class Singleton<TextureManager>;

			float getAnisotropy() const;
			void setAnisotropy(float);
			float getMaxSupportedAnisotropy() const;
		
		private:
			TextureManager();
			virtual ~TextureManager() = default;

			void clampToMaxAnisotropy();

			float anisotropy;
	};

}

#endif
