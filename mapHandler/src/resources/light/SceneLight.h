#pragma once

#include <string>
#include <Urchin3dEngine.h>

namespace urchin {

    /**
    * Represent a light on the scene
    */
    class SceneLight {
        public:
            friend class Map;

            SceneLight();
            ~SceneLight();

            void setLightManager(LightManager&);

            std::string getName() const;
            void setName(const std::string&);

            Light* getLight() const;
            void setLight(Light*);

        private:
            void loadFrom(const UdaChunk*, const DataParser&);
            void writeOn(UdaChunk&, UdaWriter&) const;

            static constexpr char NAME_ATTR[] = "name";

            LightManager* lightManager;

            std::string name;
            Light* light;
    };

}
