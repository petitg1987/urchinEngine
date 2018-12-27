#ifndef URCHINENGINE_SCENELIGHT_H
#define URCHINENGINE_SCENELIGHT_H

#include <string>
#include "Urchin3dEngine.h"

namespace urchin
{

	/**
	* Represent a light on the scene
	*/
	class SceneLight
	{
		//XML attributes
		#define NAME_ATTR "name"

		public:
			friend class Map;

			SceneLight();
			~SceneLight();

			void setLightManager(LightManager *);

			std::string getName() const;
			void setName(const std::string &);

			Light *getLight() const;
			void setLight(Light *);

		private:
			void loadFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &);
			void writeOn(const std::shared_ptr<XmlChunk> &, XmlWriter &) const;

			LightManager *lightManager;

			std::string name;
			Light *light;
	};

}

#endif
