#pragma once

#include <string>
#include <Urchin3dEngine.h>

namespace urchin {

    class LightEntity { //TODO remove
        public:
            friend class Map;

            LightEntity();
            ~LightEntity();

            std::string getName() const;
            void setName(std::string);

            Light* getLight() const;
            std::shared_ptr<Light>& getLightPtr();
            void setLight(std::shared_ptr<Light>);

        private:
            void setup(Renderer3d*);

            Renderer3d* renderer3d;

            std::string name;
            std::shared_ptr<Light> light;
    };

}
