#ifndef ENGINE_MATERIAL_H
#define ENGINE_MATERIAL_H

#include <vector>

#include "resources/Resource.h"
#include "resources/image/Image.h"

namespace urchin
{

	class Material : public Resource
	{
		public:
			Material(Image *, Image *, float);
			~Material();

			const Image *getDiffuseTexture() const;
			const Image *getNormalTexture() const;
			float getAmbientFactor() const;
		
			std::vector<const Image *> getTextures() const;

		private:
			Image *diffuseTexture;
			Image *normalTexture;
			float ambientFactor;
	};

}
	
#endif
